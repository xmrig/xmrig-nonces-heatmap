/* XMRig
 * Copyright 2014-2019 heapwolf    <https://github.com/heapwolf>
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


#ifndef XMRIG_HTTPCLIENT_H
#define XMRIG_HTTPCLIENT_H


#include "base/kernel/interfaces/IDnsListener.h"
#include "base/kernel/interfaces/ITimerListener.h"
#include "base/net/http/Fetch.h"
#include "base/net/http/HttpContext.h"
#include "base/tools/Object.h"


namespace xmrig {


class String;


class HttpClient : public HttpContext, public IDnsListener, public ITimerListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(HttpClient);

    HttpClient(const char *tag, FetchRequest &&req, const std::weak_ptr<IHttpListener> &listener);
    ~HttpClient() override;

    inline bool isQuiet() const                 { return m_req.quiet; }
    inline const char *host() const override    { return m_req.host; }
    inline const char *tag() const              { return m_tag; }
    inline uint16_t port() const override       { return m_req.port; }

    bool connect();

protected:
    void onResolved(const Dns &dns, int status) override;
    void onTimer(const Timer *timer) override;

    virtual void handshake();
    virtual void read(const char *data, size_t size);

protected:
    inline const FetchRequest &req() const  { return m_req; }

private:
    static void onConnect(uv_connect_t *req, int status);

    const char *m_tag;
    Dns *m_dns;
    FetchRequest m_req;
    Timer *m_timer  = nullptr;
};


} // namespace xmrig


#endif // XMRIG_HTTPCLIENT_H

