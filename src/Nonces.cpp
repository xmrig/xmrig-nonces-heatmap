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


#include "Nonces.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/Env.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"
#include "base/kernel/Process.h"
#include "base/tools/Chrono.h"


#include <cinttypes>


namespace xmrig {

static const char *tag = CYAN_BG_BOLD(WHITE_BOLD_S " nonces  ");

} // namespace xmrig


bool xmrig::Nonces::save()
{
    using namespace rapidjson;

    Document doc(kArrayType);
    auto &allocator = doc.GetAllocator();

    doc.Reserve(m_data.size(), allocator);
    for (uint32_t nonce : m_data) {
        doc.PushBack(nonce, allocator);
    }

    return Json::save(m_file, doc);
}


void xmrig::Nonces::load(const String &fileName)
{
    if (fileName.isEmpty()) {
        m_file = Process::location(Process::DataLocation, "nonces.json");
    }
    else {
        m_file = Env::expand(fileName);
    }

    load();
}


void xmrig::Nonces::load()
{
    using namespace rapidjson;
    const uint64_t ts = Chrono::steadyMSecs();

    Document doc;
    if (!Json::get(m_file, doc) || !doc.IsArray()) {
        return;
    }

    m_data.resize(doc.Size());

    size_t i = 0;
    for (const auto &nonce : doc.GetArray()) {
        m_data[i++] = nonce.GetUint();
    }

    if (!m_data.empty()) {
        LOG_INFO("%s " GREEN_BOLD("loaded ") CYAN_BOLD("%zu nonces ") BLACK_BOLD("(%" PRIu64 "ms)"), tag, m_data.size(), Chrono::steadyMSecs() - ts);
    }
}
