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

#ifndef XMRIG_CONFIG_H
#define XMRIG_CONFIG_H


#include "base/kernel/config/ConfigFile.h"
#include "Daemon.h"
#include "Job.h"


namespace xmrig {


class Config : public ConfigFile
{
public:
    static const char *kAutosave;
    static const char *kColors;
    static const char *kDaemon;
    static const char *kHeatmap;
    static const char *kNoncesFile;
    static const char *kOffline;
    static const char *kVerbose;

    Config() = default;

    inline bool isAutoSave() const              { return m_autoSave; }
    inline bool isOffline() const               { return m_offline || !m_daemon.isValid(); }
    inline const Daemon &daemon() const         { return m_daemon; }
    inline const Job &job() const               { return m_job; }
    inline const String &noncesFile() const     { return m_noncesFile; }

protected:
    bool parse(const IJsonReader &reader) override;
    void getJSON(rapidjson::Value &out, rapidjson::Document &doc) const override;

private:
    void setVerbose(const rapidjson::Value &value);

    bool m_autoSave         = true;
    bool m_offline          = false;
    Daemon m_daemon;
    Job m_job;
    String m_noncesFile;
};


} // namespace xmrig


#endif /* XMRIG_CONFIG_H */
