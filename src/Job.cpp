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


#include "Job.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


namespace xmrig {

const char *Job::kCount         = "count";
const char *Job::kEnd           = "end";
const char *Job::kHeight        = "height";
const char *Job::kInfoFile      = "info_file";
const char *Job::kMaxNonce      = "max_nonce";
const char *Job::kMinNonce      = "min_nonce";
const char *Job::kName          = "name";
const char *Job::kRadius        = "radius";
const char *Job::kStart         = "start";
const char *Job::kWidth         = "width";
const char *Job::kX             = "x";

} // namespace xmrig


xmrig::Job::Job(const rapidjson::Value &value)
{
    if (!value.IsObject() || value.ObjectEmpty()) {
        m_name = "heatmap.png";

        return;
    }

    m_name      = Json::getString(value, kName);
    m_infoFile  = Json::getString(value, kInfoFile);
    m_x         = Json::getUint(value, kX);
    m_height    = Json::getUint(value, kHeight);
    m_radius    = Json::getUint(value, kRadius);
    m_start     = Json::getInt(value, kStart);
    m_end       = Json::getInt(value, kEnd);
    m_minNonce  = Json::getUint(value, kMinNonce);
    m_maxNonce  = Json::getUint(value, kMaxNonce);
}


rapidjson::Value xmrig::Job::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value out(kObjectType);
    out.AddMember(StringRef(kName),     m_name.toJSON(), allocator);
    out.AddMember(StringRef(kInfoFile), m_infoFile.toJSON(), allocator);
    out.AddMember(StringRef(kX),        m_x, allocator);
    out.AddMember(StringRef(kHeight),   m_height, allocator);
    out.AddMember(StringRef(kRadius),   m_radius, allocator);
    out.AddMember(StringRef(kStart),    m_start, allocator);
    out.AddMember(StringRef(kEnd),      m_end, allocator);
    out.AddMember(StringRef(kMinNonce), m_minNonce, allocator);
    out.AddMember(StringRef(kMaxNonce), m_maxNonce, allocator);

    return out;
}
