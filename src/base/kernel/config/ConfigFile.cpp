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


#include "base/kernel/config/ConfigFile.h"
#include "3rdparty/rapidjson/document.h"
#include "3rdparty/rapidjson/error/en.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"


bool xmrig::ConfigFile::read(const String &fileName)
{
    if (fileName.isEmpty()) {
        return false;
    }

    m_fileName = fileName;
    rapidjson::Document doc;

    if (Json::get(m_fileName, doc)) {
        return parse(JsonReader(doc));
    }

    if (doc.HasParseError()) {
        const size_t offset = doc.GetErrorOffset();

        size_t line;
        size_t pos;
        std::vector<std::string> s;
        if (Json::convertOffset(fileName, offset, line, pos, s)) {
            for (const auto &t : s) {
                LOG_ERR("%s", t.c_str());
            }

            std::string t;
            if (pos > 0) {
                t.assign(pos - 1, ' ');
            }

            t += '^';

            LOG_EMERG("%s", t.c_str());
            LOG_EMERG("%s<line:%zu, position:%zu>: \"%s\"", m_fileName.data(), line, pos, GetParseError_En(doc.GetParseError()));
        }
        else {
            LOG_EMERG("%s<offset:%zu>: \"%s\"", m_fileName.data(), offset, GetParseError_En(doc.GetParseError()));
        }

        return false;
    }

    doc.SetObject();

    return parse(JsonReader(doc));
}


bool xmrig::ConfigFile::save()
{
    rapidjson::Document doc(rapidjson::kObjectType);

    getJSON(doc, doc);

    if (Json::save(m_fileName, doc)) {
        LOG_NOTICE("%s " WHITE_BOLD("\"%s\" ") GREEN_BOLD("saved"), Tags::config(), m_fileName.data());

        return true;
    }

    LOG_ERR("%s " WHITE_BOLD("\"%s\" ") RED_BOLD("save failed"), Tags::config(), m_fileName.data());

    return false;
}
