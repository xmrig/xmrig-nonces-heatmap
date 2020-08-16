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


#include "base/kernel/Entry.h"
#include "base/kernel/Process.h"
#include "base/kernel/config/usage.h"
#include "version.h"


#include <cstdio>
#include <uv.h>


#ifdef XMRIG_SODIUM
#   include "base/tools/Sodium.h"
#   include <sodium.h>
#endif


namespace xmrig {


static int showVersion()
{
    printf(APP_NAME " " APP_VERSION "\n built on " __DATE__

#   if defined(__clang__)
    " with clang " __clang_version__);
#   elif defined(__GNUC__)
    " with GCC");
    printf(" %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   elif defined(_MSC_VER)
    " with MSVC");
    printf(" %d", MSVC_VERSION);
#   else
    );
#   endif

    printf("\n");

    return 0;
}


#ifdef XMRIG_SODIUM
static int genKey()
{
    uint8_t key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

    crypto_secretstream_xchacha20poly1305_keygen(key);

    printf("%s\n", Sodium::toBase64(key, sizeof (key)).data());

    return 0;
}
#endif


} // namespace xmrig


xmrig::Entry::Id xmrig::Entry::get(const Process &process)
{
    const Arguments &args = process.arguments();
    if (args.hasArg("-h") || args.hasArg("--help")) {
         return Usage;
    }

    if (args.hasArg("-V") || args.hasArg("--version")) {
         return Version;
    }

    if (args.hasArg("--gen-key")) {
        return GenKey;
    }

    return Default;
}


int xmrig::Entry::exec(const Process &, Id id)
{
    switch (id) {
    case Usage:
        printf("%s\n", usage().c_str());
        return 0;

    case Version:
        return showVersion();

#   ifdef XMRIG_SODIUM
    case GenKey:
        return genKey();
#   endif

    default:
        break;
    }

    return 1;
}
