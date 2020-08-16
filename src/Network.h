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

#ifndef XMRIG_NETWORK_H
#define XMRIG_NETWORK_H


#include "base/kernel/interfaces/IHttpListener.h"
#include "base/kernel/interfaces/ITimerListener.h"
#include "base/tools/String.h"


#include <algorithm>
#include <memory>
#include <string>
#include <map>


namespace xmrig {


class Daemon;
class IJsonReader;
class INetworkListener;


class Range
{
public:
    Range() = default;
    Range(uint32_t start, uint32_t count, uint32_t max) : start(start), end(std::min(start + count, max)) {}

    const uint32_t start  = 0;
    const uint32_t end    = 0;
};


class Network : public IHttpListener, public ITimerListener
{
public:
    Network(const Daemon &daemon, std::vector<uint32_t> &nonces, INetworkListener *listener);

    void connect();

    static const char *tag();

protected:
    inline void onTimer(const Timer *) override     { printSync(); }

    void onHttpData(const HttpData &data) override;

private:
    enum RequestType : int {
        LAST_BLOCK_HEADER   = 1,
        BLOCK_HEADERS_RANGE = 2
    };

    bool setBlockHeadersRange(uint64_t id, const rapidjson::Value &headers);
    bool setLastBlockHeader(const IJsonReader &reader);
    uint64_t getBlockHeadersRange();
    uint64_t getBlockHeadersRange(const Range &range);
    uint64_t getLastBlockHeader();
    uint64_t rpcSend(const rapidjson::Value &value, int type, uint64_t id);
    void onRpcResponse(uint64_t id, const rapidjson::Value &result, const rapidjson::Value &error, int type);
    void printSync();
    void save();
    void sync();

    const Daemon &m_daemon;
    const std::string m_url;
    INetworkListener *m_listener;
    std::map<uint32_t, uint32_t> m_data;
    std::map<uint64_t, Range> m_req;
    std::shared_ptr<IHttpListener> m_httpListener;
    std::vector<uint32_t> &m_nonces;
    std::shared_ptr<Timer> m_timer;
    uint32_t m_height       = 0;
    uint32_t m_pos          = 0;
    uint64_t m_timestamp    = 0;
};


} // namespace xmrig


#endif /* XMRIG_NETWORK_H */
