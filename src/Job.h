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

#ifndef XMRIG_JOB_H
#define XMRIG_JOB_H


#include "base/tools/String.h"


namespace xmrig {


class Job
{
public:
    static const char *kCount;
    static const char *kEnd;
    static const char *kHeight;
    static const char *kInfoFile;
    static const char *kMaxNonce;
    static const char *kMinNonce;
    static const char *kName;
    static const char *kRadius;
    static const char *kStart;
    static const char *kWidth;
    static const char *kX;

    Job() = default;
    Job(const rapidjson::Value &value);

    inline const String &infoFile() const               { return m_infoFile; }
    inline const String &name() const                   { return m_name; }
    inline int32_t end() const                          { return m_end; }
    inline int32_t start() const                        { return m_start; }
    inline uint32_t height() const                      { return m_height; }
    inline uint32_t maxNonce() const                    { return m_maxNonce; }
    inline uint32_t minNonce() const                    { return m_minNonce; }
    inline uint32_t radius() const                      { return m_radius; }
    inline uint32_t x() const                           { return m_x; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    int32_t m_end       = 0;
    int32_t m_start     = 0;
    String m_infoFile;
    String m_name;
    uint32_t m_height   = 4096;
    uint32_t m_maxNonce = 0;
    uint32_t m_minNonce = 0;
    uint32_t m_radius   = 5;
    uint32_t m_x        = 0;
};


} // namespace xmrig


#endif /* XMRIG_JOB_H */
