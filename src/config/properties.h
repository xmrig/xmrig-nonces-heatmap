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

#ifndef XMRIG_PROPERTIES_H
#define XMRIG_PROPERTIES_H


#include <cstddef>
#include <cstdint>


constexpr const char *XMRIG_DEFAULT_CONFIG_FILE_NAME        = "config.json";

//constexpr const char *XMRIG_TCP_ANY_BIND_IP                 = "0.0.0.0";
//constexpr uint64_t    XMRIG_TCP_CONNECT_TIMEOUT             = 20 * 1000;
//constexpr uint64_t    XMRIG_TCP_HANDSHAKE_TIMEOUT           = 20 * 1000;
//constexpr uint64_t    XMRIG_TCP_RECONNECT_TIMEOUT           = 1000;
//constexpr uint64_t    XMRIG_TCP_RESPONSE_TIMEOUT            = XMRIG_TCP_HANDSHAKE_TIMEOUT;

//constexpr const char *XMRIG_RPC_BIND_IP                     = "0.0.0.0";
//constexpr const char *XMRIG_RPC_DEFAULT_NAME                = "${XMRIG_HOSTNAME}";
//constexpr uint16_t    XMRIG_RPC_BIND_PORT                   = 18801;
//constexpr uint32_t    XMRIG_RPC_HELLO_MAX_SIZE              = 8 * 1024;
//constexpr uint64_t    XMRIG_RPC_PEER_TIMEOUT                = XMRIG_TCP_RESPONSE_TIMEOUT * 2;
//constexpr uint64_t    XMRIG_RPC_PING_TIMEOUT                = XMRIG_TCP_RESPONSE_TIMEOUT;

constexpr size_t      XMRIG_NET_BUFFER_CHUNK_SIZE           = 64 * 1024;
constexpr size_t      XMRIG_NET_BUFFER_INIT_CHUNKS          = 4;

//constexpr bool        XMRIG_DB_REQUIRED                     = true;
//constexpr const char *XMRIG_DB_FILE_NAME                    = "db.sqlite";


#endif /* XMRIG_PROPERTIES_H */
