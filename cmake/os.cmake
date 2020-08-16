if (WIN32)
    set(XMRIG_OS_WIN ON)
elseif (APPLE)
    set(XMRIG_OS_APPLE ON)

    if (IOS OR CMAKE_SYSTEM_NAME STREQUAL iOS)
        set(XMRIG_OS_IOS ON)
    else()
        set(XMRIG_OS_MACOS ON)
    endif()
else()
    set(XMRIG_OS_UNIX ON)

    if (ANDROID OR CMAKE_SYSTEM_NAME MATCHES "Android")
        set(XMRIG_OS_ANDROID ON)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(XMRIG_OS_LINUX ON)
    elseif(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
        set(XMRIG_OS_FREEBSD ON)
    endif()
endif()


if (XMRIG_OS_WIN)
    add_definitions(/DWIN32)
    add_definitions(/DXMRIG_OS_WIN)
    set(EXTRA_LIBS ws2_32 psapi iphlpapi userenv)
elseif(XMRIG_OS_APPLE)
    add_definitions(/DXMRIG_OS_APPLE)

    if (XMRIG_OS_IOS)
        add_definitions(/DXMRIG_OS_IOS)
    else()
        add_definitions(/DXMRIG_OS_MACOS)
    endif()
elseif(XMRIG_OS_UNIX)
    add_definitions(/DXMRIG_OS_UNIX)
    set(EXTRA_LIBS pthread)

    if (XMRIG_OS_ANDROID)
        add_definitions(/DXMRIG_OS_ANDROID)
    elseif (XMRIG_OS_LINUX)
        add_definitions(/DXMRIG_OS_LINUX)
        list(APPEND EXTRA_LIBS rt dl)
    elseif (XMRIG_OS_FREEBSD)
        add_definitions(/DXMRIG_OS_FREEBSD)
    endif()
endif()
