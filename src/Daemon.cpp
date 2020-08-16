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


#include "Daemon.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


#include <algorithm>


namespace xmrig {

const char *Daemon::kConcurrency    = "concurrency";
const char *Daemon::kHost           = "host";
const char *Daemon::kMaxRange       = "max_range";
const char *Daemon::kPort           = "port";
const char *Daemon::kPrintTime      = "print_time";
const char *Daemon::kTimeout        = "timeout";
const char *Daemon::kTls            = "tls";

} // namespace xmrig


xmrig::Daemon::Daemon(const rapidjson::Value &value)
{
    if (!value.IsObject() || value.ObjectEmpty()) {
        m_host = "127.0.0.1";

        return;
    }

    m_host          = Json::getString(value, kHost);
    m_port          = Json::getUint(value, kPort);
    m_concurrency   = std::max(Json::getUint(value, kConcurrency, m_concurrency), 1U);
    m_maxRange      = std::max(Json::getUint(value, kMaxRange, m_maxRange), 1U);
    m_printTime     = Json::getUint(value, kPrintTime, m_printTime);
    m_timeout       = std::max(Json::getUint(value, kTimeout, m_timeout), 1U);

#   ifdef XMRIG_FEATURE_TLS
    m_tls = Json::getUint(value, kTls, m_tls);
#   endif
}


rapidjson::Value xmrig::Daemon::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value out(kObjectType);
    out.AddMember(StringRef(kHost),         m_host.toJSON(), allocator);
    out.AddMember(StringRef(kPort),         m_port, allocator);
    out.AddMember(StringRef(kTls),          m_tls, allocator);
    out.AddMember(StringRef(kConcurrency),  m_concurrency, allocator);
    out.AddMember(StringRef(kMaxRange),     m_maxRange, allocator);
    out.AddMember(StringRef(kPrintTime),    m_printTime, allocator);
    out.AddMember(StringRef(kTimeout),      m_timeout, allocator);

    return out;
}
