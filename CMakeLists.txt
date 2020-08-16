cmake_minimum_required(VERSION 2.8)
project(xmrig-nonces-heatmap)

option(WITH_DEBUG_LOG       "Enable debug log output" OFF)
option(WITH_ENV_VARS        "Enable environment variables support in config file" ON)
option(WITH_TLS             "Enable OpenSSL support" OFF)
option(WITH_LIBPNG          "Enable LibPNG support" OFF)

set(WITH_HTTP               ON)

set(XMRIG_SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(XMRIG_ROOT_DIR          "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH       ${CMAKE_MODULE_PATH} "${XMRIG_ROOT_DIR}/cmake")

include(${XMRIG_ROOT_DIR}/cmake/os.cmake)
include(${XMRIG_ROOT_DIR}/cmake/cpu.cmake)
include(${XMRIG_ROOT_DIR}/cmake/flags.cmake)
include(${XMRIG_ROOT_DIR}/cmake/OpenSSL.cmake)
include(${XMRIG_SOURCE_DIR}/base/base.cmake)

set(HEADERS
    src/version.h
    src/App.h
    src/config/properties.h
    src/config/Config.h
    src/Network.h
    src/Nonces.h
    src/Job.h
    src/Daemon.h
    src/Heatmap.h
    src/3rdparty/heatmap/heatmap.h
    src/3rdparty/heatmap/lodepng.h
    src/3rdparty/heatmap/colorschemes/Spectral.h
    src/interfaces/INetworkListener.h
   )

set(SOURCES
    src/main.cpp
    src/App.cpp
    src/config/Config.cpp
    src/Network.cpp
    src/Nonces.cpp
    src/Job.cpp
    src/Daemon.cpp
    src/Heatmap.cpp
    src/3rdparty/heatmap/heatmap.c
    src/3rdparty/heatmap/lodepng.cpp
    src/3rdparty/heatmap/colorschemes/Spectral.c
   )

if (XMRIG_OS_WIN)
    list(APPEND SOURCES
        res/app.rc
        )

    set(EXTRA_LIBS ws2_32 psapi iphlpapi userenv)
elseif(NOT XMRIG_OS_APPLE)
    if (XMRIG_OS_ANDROID)
        set(EXTRA_LIBS pthread rt dl log)
    elseif (XMRIG_OS_LINUX)
        set(EXTRA_LIBS pthread rt dl)
    elseif (XMRIG_OS_FREEBSD)
        set(EXTRA_LIBS kvm pthread)
    endif()
endif()

find_package(UV REQUIRED)

include_directories(src)

if (WITH_DEBUG_LOG)
    add_definitions(/DAPP_DEBUG)
endif()

if (WITH_LIBPNG)
    find_package(PNG REQUIRED)

    include_directories(${PNG_INCLUDE_DIRS})
    add_definitions(${PNG_DEFINITIONS} -DXMRIG_LIBPNG)
else()
    remove_definitions(-DXMRIG_LIBPNG)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS_BASE} ${HEADERS} ${SOURCES_BASE} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENSSL_LIBRARIES} ${UV_LIBRARIES} ${EXTRA_LIBS} ${PNG_LIBRARIES})
