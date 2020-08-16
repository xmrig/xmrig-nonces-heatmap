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


#include "config/Config.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/log/Log.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "config/properties.h"
#include "version.h"


#include <algorithm>


namespace xmrig {


const char *Config::kAutosave       = "autosave";
const char *Config::kColors         = "colors";
const char *Config::kDaemon         = "daemon";
const char *Config::kHeatmap        = "heatmap";
const char *Config::kNoncesFile     = "nonces_file";
const char *Config::kOffline        = "offline";
const char *Config::kVerbose        = "verbose";


} // namespace xmrig


bool xmrig::Config::parse(const IJsonReader &reader)
{
    m_offline       = reader.getBool(kOffline, m_offline);
    m_autoSave      = reader.getBool(kAutosave, m_autoSave);
    m_noncesFile    = reader.getString(kNoncesFile);
    m_job           = reader.getObject(kHeatmap);
    m_daemon        = reader.getObject(kDaemon);

    Log::setColors(reader.getBool(kColors, Log::isColors()));
    setVerbose(reader.getValue(kVerbose));

    return true;
}


void xmrig::Config::getJSON(rapidjson::Value &out, rapidjson::Document &doc) const
{
    using namespace rapidjson;

    out.SetObject();
    auto &allocator = doc.GetAllocator();

    out.AddMember(StringRef(kOffline),      isOffline(), allocator);
    out.AddMember(StringRef(kAutosave),     isAutoSave(), allocator);
    out.AddMember(StringRef(kNoncesFile),   m_noncesFile.toJSON(), allocator);
    out.AddMember(StringRef(kDaemon),       m_daemon.toJSON(doc), allocator);
    out.AddMember(StringRef(kHeatmap),      m_job.toJSON(doc), allocator);
    out.AddMember(StringRef(kColors),       Log::isColors(), allocator);
    out.AddMember(StringRef(kVerbose),      Log::verbose(), allocator);
}


void xmrig::Config::setVerbose(const rapidjson::Value &value)
{
    if (value.IsBool()) {
        Log::setVerbose(value.GetBool() ? 1 : 0);
    }
    else if (value.IsUint()) {
        Log::setVerbose(value.GetUint());
    }
}
