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

#ifndef XMRIG_NONCES_H
#define XMRIG_NONCES_H


#include "base/tools/String.h"


namespace xmrig {


class Nonces
{
public:
    Nonces() = default;

    inline const std::vector<uint32_t> &data() const    { return m_data; }
    inline const String &file() const                   { return m_file; }
    inline std::vector<uint32_t> &data()                { return m_data; }

    bool save();
    void load(const String &fileName);

private:
    void load();

    std::vector<uint32_t> m_data;
    String m_file;
};


} // namespace xmrig


#endif /* XMRIG_NONCES_H */
