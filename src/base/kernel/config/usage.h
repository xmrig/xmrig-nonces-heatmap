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

#ifndef XMRIG_USAGE_H
#define XMRIG_USAGE_H


#include "version.h"


#include <string>


namespace xmrig {


static inline const std::string &usage()
{
    static std::string u;

    if (!u.empty()) {
        return u;
    }

    u += "Usage: " APP_ID " [OPTIONS]\n\nNetwork:\n";
    u += "  -d, --data-dir=DIR      custom writable directory to store files\n";
    u += "  -c, --config=FILE       load a JSON-format configuration file\n";
#   ifdef XMRIG_SODIUM
    u += "      --gen-key           generate secret key for RPC peers\n";
#   endif
    u += "  -V, --version           output version information and exit\n";
    u += "  -h, --help              display this help and exit\n";

    return u;
}


} /* namespace xmrig */

#endif /* XMRIG_USAGE_H */
