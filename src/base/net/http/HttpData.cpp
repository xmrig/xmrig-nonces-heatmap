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


#include "base/net/http/HttpData.h"
#include "3rdparty/http-parser/http_parser.h"


#include <uv.h>


namespace xmrig {


const std::string HttpData::kApplicationJson    = "application/json";
const std::string HttpData::kContentType        = "Content-Type";
const std::string HttpData::kContentTypeL       = "content-type";
const std::string HttpData::kTextPlain          = "text/plain";


} // namespace xmrig


bool xmrig::HttpData::isJSON() const
{
    if (!headers.count(kContentTypeL)) {
        return false;
    }

    auto &type = headers.at(kContentTypeL);

    return type == kApplicationJson || type == kTextPlain;
}


const char *xmrig::HttpData::methodName() const
{
    return http_method_str(static_cast<http_method>(method));
}


const char *xmrig::HttpData::statusName() const
{
    if (status < 0) {
        return uv_strerror(status);
    }

    return http_status_str(static_cast<http_status>(status));
}
