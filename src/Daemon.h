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

#ifndef XMRIG_DAEMON_H
#define XMRIG_DAEMON_H


#include "base/tools/String.h"


namespace xmrig {


class Daemon
{
public:
    static const char *kConcurrency;
    static const char *kHost;
    static const char *kMaxRange;
    static const char *kPort;
    static const char *kPrintTime;
    static const char *kTimeout;
    static const char *kTls;

    Daemon() = default;
    Daemon(const rapidjson::Value &value);

    inline bool isTLS() const           { return m_tls; }
    inline bool isValid() const         { return !m_host.isEmpty() && m_port > 0; }
    inline const String &host() const   { return m_host; }
    inline uint16_t port() const        { return m_port; }
    inline uint32_t concurrency() const { return m_concurrency; }
    inline uint32_t maxRange()  const   { return m_maxRange; }
    inline uint64_t printTime() const   { return m_printTime * 1000; }
    inline uint64_t timeout() const     { return m_timeout * 1000; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    bool m_tls              = false;
    String m_host;
    uint16_t m_port         = 18081;
    uint32_t m_concurrency  = 10;
    uint32_t m_maxRange     = 1000;
    uint32_t m_printTime    = 10;
    uint32_t m_timeout      = 30;
};


} // namespace xmrig


#endif /* XMRIG_DAEMON_H */
