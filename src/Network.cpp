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


#include "Network.h"
#include "3rdparty/rapidjson/document.h"
#include "3rdparty/rapidjson/error/en.h"
#include "base/io/json/Json.h"
#include "base/io/json/JsonRequest.h"
#include "base/io/log/Log.h"
#include "base/net/http/Fetch.h"
#include "base/net/http/HttpData.h"
#include "base/net/http/HttpListener.h"
#include "base/tools/Chrono.h"
#include "base/tools/Timer.h"
#include "Daemon.h"
#include "interfaces/INetworkListener.h"


namespace xmrig {


static const char *kTag = BLUE_BG_BOLD(WHITE_BOLD_S " network ");


} // namespace xmrig



xmrig::Network::Network(const Daemon &daemon, std::vector<uint32_t> &nonces, INetworkListener *listener) :
    m_daemon(daemon),
    m_url(std::string("http") + (daemon.isTLS() ? "s" : "") + "://" + daemon.host().data() + ":" + std::to_string(daemon.port())),
    m_listener(listener),
    m_nonces(nonces)
{
    m_httpListener = std::make_shared<HttpListener>(this, tag());
}


void xmrig::Network::connect()
{
    LOG_INFO("%s " MAGENTA_BOLD("use daemon ") CYAN_BOLD("%s"), tag(), m_url.c_str());

    getLastBlockHeader();
}


const char *xmrig::Network::tag()
{
    return kTag;
}


void xmrig::Network::onHttpData(const HttpData &data)
{
    using namespace rapidjson;

    if (data.status != HTTP_STATUS_OK) {
        LOG_ERR("%s " MAGENTA_BOLD("\"%s %s\" ") RED("%d ") RED_BOLD("\"%s\""), tag(), data.methodName(), data.url.data(), data.status, data.statusName());

        if (data.userType == BLOCK_HEADERS_RANGE && data.status < 0) {
            auto it = m_req.find(data.rpcId);
            if (it != m_req.end()) {
                getBlockHeadersRange(it->second);
                m_req.erase(it);

                return;
            }
        }
    }

    if (!data.isJSON()) {
        return m_listener->onFatalError();
    }

    rapidjson::Document doc;
    if (doc.Parse(data.body.c_str()).HasParseError()) {
        LOG_ERR("%s " RED("JSON decode failed: ") RED_BOLD("\"%s\""), tag(), GetParseError_En(doc.GetParseError()));

        return m_listener->onFatalError();
    }

    onRpcResponse(Json::getUint64(doc, "id"), Json::getObject(doc, "result"), Json::getObject(doc, "error"), data.userType);
}


bool xmrig::Network::setBlockHeadersRange(uint64_t id, const rapidjson::Value &headers)
{
    if (!headers.IsArray() || headers.Empty()) {
        return false;
    }

    auto it = m_req.find(id);
    if (it == m_req.end()) {
        return false;
    }

    for (const auto &header : headers.GetArray()) {
        m_data.insert({ Json::getUint(header, "height"), Json::getUint(header, "nonce") });
    }

    m_req.erase(it);
    if (m_req.empty() && m_pos > m_height) {
        save();

        return true;
    }

    if (m_req.size() < m_daemon.concurrency()) {
        getBlockHeadersRange();
    }

    return true;
}


bool xmrig::Network::setLastBlockHeader(const IJsonReader &reader)
{
    m_height = reader.getUint("height");

    if (!m_height || m_nonces.size() > (m_height + 1)) {
        LOG_ERR("%s " RED_BOLD("wrong height: %u"), tag(), m_height);

        return false;
    }

    if (m_nonces.size() < (m_height + 1)) {
        sync();

        return true;
    }

    LOG_INFO("%s " GREEN_BOLD("already up to date"), tag());
    m_listener->onSyncDone(false);

    return true;
}


uint64_t xmrig::Network::getBlockHeadersRange()
{
    if (m_pos > m_height) {
        return 0;
    }

    return getBlockHeadersRange(Range(m_pos, m_daemon.maxRange(), m_height));
}


uint64_t xmrig::Network::getBlockHeadersRange(const Range &range)
{
    using namespace rapidjson;
    Document doc(kObjectType);
    auto &allocator = doc.GetAllocator();

    Value params(kObjectType);
    params.AddMember("start_height", range.start, allocator);
    params.AddMember("end_height",   range.end, allocator);

    m_pos = range.end + 1;

    LOG_V4("%s " MAGENTA_BOLD("get range ") CYAN_BOLD("%u-%u") " remaining " CYAN_BOLD("%u"), tag(), range.start, range.end, m_height - range.start + 1);

    auto id = rpcSend(doc, BLOCK_HEADERS_RANGE, JsonRequest::create(doc, "getblockheadersrange", params));
    m_req.insert({ id, range });

    return id;
}


uint64_t xmrig::Network::getLastBlockHeader()
{
    using namespace rapidjson;
    Document doc(kObjectType);

    return rpcSend(doc, LAST_BLOCK_HEADER, JsonRequest::create(doc, "getlastblockheader", Value(kNullType).Move()));
}


uint64_t xmrig::Network::rpcSend(const rapidjson::Value &value, int type, uint64_t id)
{
    LOG_V5("%s " WHITE_BOLD("#%" PRIu64) MAGENTA_BOLD(" %s"), tag(), id, Json::getString(value, "method"));

    FetchRequest req(HTTP_POST, m_daemon.host(), m_daemon.port(), "/json_rpc", value, m_daemon.isTLS());
    req.timeout = m_daemon.timeout();

    fetch(tag(), std::move(req), m_httpListener, type, id);

    return id;
}


void xmrig::Network::onRpcResponse(uint64_t id, const rapidjson::Value &result, const rapidjson::Value &error, int type)
{
    if (error.IsObject()) {
        LOG_ERR("%s " WHITE_BOLD("#%" PRIu64) RED(" RPC error: ") RED_BOLD("\"%s\""), tag(), id, Json::getString(error, "message"));

        return m_listener->onFatalError();
    }

    if (!result.IsObject()) {
        return m_listener->onFatalError();
    }

    if (type == LAST_BLOCK_HEADER && !setLastBlockHeader(JsonReader(Json::getObject(result, "block_header")))) {
        return m_listener->onFatalError();
    }

    if (type == BLOCK_HEADERS_RANGE && !setBlockHeadersRange(id, Json::getArray(result, "headers"))) {
        return m_listener->onFatalError();
    }
}


void xmrig::Network::sync()
{
    if (m_daemon.printTime()) {
        m_timer = std::make_shared<Timer>(this, m_daemon.printTime(), m_daemon.printTime());
    }

    m_timestamp = Chrono::steadyMSecs();
    m_pos       = m_nonces.size();

    printSync();

    for (uint32_t i = 0; i < m_daemon.concurrency(); ++i) {
        getBlockHeadersRange();
    }
}


void xmrig::Network::printSync()
{
    const uint32_t synced = m_nonces.size() + m_data.size();
    const uint32_t total  = m_height + 1;
    const double percent  = static_cast<double>(synced) / total * 100.0;

    LOG_INFO("%s " MAGENTA_BOLD("sync ") YELLOW_BOLD("%5.2f%% ") CYAN_BOLD("%u/%u") " remaining " CYAN_BOLD("%zu"), tag(), percent, synced, total, total - synced);
}


void xmrig::Network::save()
{
    m_timer.reset();
    m_nonces.reserve(m_nonces.size() + m_data.size());

    for (size_t i = m_nonces.size(); i <= m_height; ++i) {
        m_nonces.emplace_back(m_data.at(i));
    }

    LOG_INFO("%s " GREEN_BOLD("done ") BLACK_BOLD("(%" PRIu64 "ms)"), tag(), Chrono::steadyMSecs() - m_timestamp);

    m_data.clear();
    m_listener->onSyncDone(true);
}
