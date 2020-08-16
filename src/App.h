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

#ifndef XMRIG_APP_H
#define XMRIG_APP_H


#include "base/kernel/interfaces/IConsoleListener.h"
#include "base/kernel/interfaces/ISignalListener.h"
#include "interfaces/INetworkListener.h"
#include "Nonces.h"


#include <memory>


namespace xmrig {


class Config;
class Console;
class Network;
class Process;
class Signals;


class App : public IConsoleListener, public ISignalListener, public INetworkListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(App)

    App(const Process &process);
    ~App() override;

    int exec();
    void close(int rc = 0);

    inline Config *config() const           { return m_config; }
    inline const Process &process() const   { return m_process; }

protected:
    void onConsoleCommand(char command) override;
    void onFatalError(const char *message = nullptr) override;
    void onSignal(int signum) override;
    void onSyncDone(bool modified) override;

private:
    bool render();
    void initLog();

    Config *m_config    = nullptr;
    const Process &m_process;
    int m_rc            = 0;
    Nonces m_nonces;
    std::shared_ptr<Console> m_console;
    std::shared_ptr<Network> m_network;
    std::shared_ptr<Signals> m_signals;
};


} /* namespace xmrig */


#endif /* XMRIG_APP_H */
