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

#ifndef XMRIG_INETWORKLISTENER_H
#define XMRIG_INETWORKLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class INetworkListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(INetworkListener)

    INetworkListener()          = default;
    virtual ~INetworkListener() = default;

    virtual void onFatalError(const char *message = nullptr)    = 0;
    virtual void onSyncDone(bool modified)                      = 0;
};


} /* namespace xmrig */


#endif // XMRIG_INETWORKLISTENER_H
