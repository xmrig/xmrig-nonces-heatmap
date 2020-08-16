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

#ifndef XMRIG_HEATMAP_H
#define XMRIG_HEATMAP_H


#include "base/tools/String.h"


namespace xmrig {


class Job;


class Heatmap
{
public:
    Heatmap(const Job &job, const std::vector<uint32_t> &nonces);

    void render();

    static const char *tag();

private:
    void genName(const std::vector<uint32_t>::const_iterator &begin, const std::vector<uint32_t>::const_iterator &end);
    void saveInfo();

    const Job &m_job;
    const std::vector<uint32_t> &m_nonces;
    String m_name;
    uint32_t m_count    = 0;
    uint32_t m_end      = 0;
    uint32_t m_start    = 0;
    uint32_t m_width    = 0;
    uint32_t m_x        = 0;
};


} // namespace xmrig


#endif /* XMRIG_HEATMAP_H */
