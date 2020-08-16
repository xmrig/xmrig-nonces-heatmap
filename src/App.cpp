/* XMRig
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <cstdlib>
#include <uv.h>


#include "App.h"
#include "base/io/Console.h"
#include "base/io/log/backends/ConsoleLog.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/io/Signals.h"
#include "base/kernel/Platform.h"
#include "base/kernel/Process.h"
#include "base/net/tools/NetBuffer.h"
#include "config/Config.h"
#include "config/properties.h"
#include "Heatmap.h"
#include "Network.h"
#include "version.h"


namespace xmrig {


static String getConfigName(const Process &process)
{
    auto name = process.arguments().value("--config", "-c");
    if (name) {
        return name;
    }

    return Process::location(Process::DataLocation, XMRIG_DEFAULT_CONFIG_FILE_NAME);
}


} // namespace xmrig



xmrig::App::App(const Process &process) :
    m_process(process)
{
    auto config = new Config();
    if (!config->read(getConfigName(process))) {
        delete config;

        return;
    }

    m_config = config;
}


xmrig::App::~App()
{
    delete m_config;

    NetBuffer::destroy();
}


int xmrig::App::exec()
{
    if (!m_config) {
        LOG_EMERG("no valid configuration found");

        return 2;
    }

    Platform::init(nullptr);

    m_signals = std::make_shared<Signals>(this);

    int rc = 0;

    initLog();

    LOG_NOTICE(GREEN_BOLD("%s/%s") " %.*s " CYAN_BOLD("%s"), APP_NAME, APP_VERSION, 13, APP_COPYRIGHT + 10, APP_DOMAIN);

    m_nonces.load(m_config->noncesFile());

    if (m_config->isAutoSave()) {
        m_config->save();
    }

    if (m_config->isOffline()) {
        close(render() ? 0 : 1);

        uv_run(uv_default_loop(), UV_RUN_ONCE);
        uv_loop_close(uv_default_loop());

        return m_rc;
    }

    m_network = std::make_shared<Network>(m_config->daemon(), m_nonces.data(), this);
    m_network->connect();

    rc = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());

    return rc == 0 ? m_rc : rc;
}


void xmrig::App::close(int rc)
{
    m_rc = rc;

    m_network.reset();
    m_signals.reset();
    m_console.reset();

    Log::destroy();
}


void xmrig::App::onConsoleCommand(char command)
{
    if (command == 3) {
        LOG_WARN("%s " YELLOW("Ctrl+C received, exiting"), Tags::signal());
        close();
    }
}


void xmrig::App::onFatalError(const char *message)
{
    if (message) {
        LOG_EMERG("%s " RED_BOLD("%s"), Network::tag(), message);
    }

    LOG_ERR("%s " RED_BOLD("fatal error"), Tags::signal());

    close(1);
}


void xmrig::App::onSignal(int signum)
{
    switch (signum)
    {
    case SIGHUP:
        LOG_WARN("%s " YELLOW("SIGHUP received, exiting"), Tags::signal());
        break;

    case SIGTERM:
        LOG_WARN("%s " YELLOW("SIGTERM received, exiting"), Tags::signal());
        break;

    case SIGINT:
        LOG_WARN("%s " YELLOW("SIGINT received, exiting"), Tags::signal());
        break;

    default:
        return;
    }

    close();
}


void xmrig::App::onSyncDone(bool modified)
{
    if (modified) {
        m_nonces.save();
    }

    close(render() ? 0 : 1);
}


bool xmrig::App::render()
{
    auto heatmap = std::make_shared<Heatmap>(m_config->job(), m_nonces.data());

    try {
        heatmap->render();
    } catch (std::exception &ex) {

        LOG_EMERG("%s " RED_BOLD("%s"), Heatmap::tag(), ex.what());

        return false;
    }

    return true;
}


void xmrig::App::initLog()
{
    Log::add(new ConsoleLog());
    m_console = std::make_shared<Console>(this);
}
