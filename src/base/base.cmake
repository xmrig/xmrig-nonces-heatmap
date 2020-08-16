find_package(UV REQUIRED)

include_directories(${UV_INCLUDE_DIR})

set(HEADERS_BASE
    ${CMAKE_CURRENT_LIST_DIR}/io/Console.h
    ${CMAKE_CURRENT_LIST_DIR}/io/Env.h
    ${CMAKE_CURRENT_LIST_DIR}/io/json/Json.h
    ${CMAKE_CURRENT_LIST_DIR}/io/json/JsonRequest.h
    ${CMAKE_CURRENT_LIST_DIR}/io/log/backends/ConsoleLog.h
    ${CMAKE_CURRENT_LIST_DIR}/io/log/Log.h
    ${CMAKE_CURRENT_LIST_DIR}/io/log/Tags.h
    ${CMAKE_CURRENT_LIST_DIR}/io/Signals.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/config/ConfigFile.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/config/usage.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Entry.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/IConsoleListener.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/IJsonReader.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/ILogBackend.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/ISignalListener.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/ITimerListener.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Platform.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Process.h
    ${CMAKE_CURRENT_LIST_DIR}/kernel/version-common.h
    ${CMAKE_CURRENT_LIST_DIR}/net/tools/MemPool.h
    ${CMAKE_CURRENT_LIST_DIR}/net/tools/NetBuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/net/tools/Storage.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Arguments.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Baton.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Buffer.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Chrono.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Cvt.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Handle.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Object.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/String.h
    ${CMAKE_CURRENT_LIST_DIR}/tools/Timer.h
   )

set(SOURCES_BASE
    ${CMAKE_CURRENT_LIST_DIR}/io/Console.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/Env.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/json/Json.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/json/JsonRequest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/log/backends/ConsoleLog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/log/Log.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/log/Tags.cpp
    ${CMAKE_CURRENT_LIST_DIR}/io/Signals.cpp
    ${CMAKE_CURRENT_LIST_DIR}/kernel/config/ConfigFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Entry.cpp
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Platform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/kernel/Process.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/tools/NetBuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tools/Arguments.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tools/Cvt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tools/String.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tools/Timer.cpp
   )


if (WIN32)
    list(APPEND SOURCES_BASE
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Process_win.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/json/Json_win.cpp
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Platform_win.cpp
        )
elseif (APPLE)
    list(APPEND SOURCES_BASE
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Process_unix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/json/Json_unix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Platform_mac.cpp
        )
else()
    list(APPEND SOURCES_BASE
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Process_unix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/json/Json_unix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/kernel/Platform_unix.cpp
        )
endif()


add_definitions(-DUNICODE -D__STDC_FORMAT_MACROS)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(/DRAPIDJSON_SSE2)
endif()


if (WITH_DEBUG_LOG)
    add_definitions(-DAPP_DEBUG)
endif()


if (WITH_ENV_VARS AND CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
    set(WITH_ENV_VARS OFF)
endif()


if (WITH_ENV_VARS)
    add_definitions(-DXMRIG_FEATURE_ENV)
else()
    remove_definitions(-DXMRIG_FEATURE_ENV)
endif()


if (WITH_HTTP)
    list(APPEND HEADERS_BASE
        ${XMRIG_SOURCE_DIR}/3rdparty/http-parser/http_parser.h
        ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/IDnsListener.h
        ${CMAKE_CURRENT_LIST_DIR}/kernel/interfaces/IHttpListener.h
        ${CMAKE_CURRENT_LIST_DIR}/net/dns/Dns.h
        ${CMAKE_CURRENT_LIST_DIR}/net/dns/DnsRecord.h
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpClient.h
        ${CMAKE_CURRENT_LIST_DIR}/net/http/Fetch.h
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpContext.h
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpData.h
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpListener.h
        )

    list(APPEND SOURCES_BASE
        ${XMRIG_SOURCE_DIR}/3rdparty/http-parser/http_parser.c
        ${CMAKE_CURRENT_LIST_DIR}/net/dns/Dns.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/dns/DnsRecord.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/http/Fetch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/http/HttpListener.cpp
        )

    if (WITH_TLS)
        list(APPEND HEADERS_BASE
            ${CMAKE_CURRENT_LIST_DIR}/net/https/HttpsClient.h
            )

        list(APPEND SOURCES_BASE
            ${CMAKE_CURRENT_LIST_DIR}/net/https/HttpsClient.cpp
            )
    endif()

    add_definitions(/DXMRIG_FEATURE_HTTP)
else()
    remove_definitions(/DXMRIG_FEATURE_HTTP)
endif()


execute_process(COMMAND git log -1 --format=%h WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
if (NOT GIT_COMMIT_HASH STREQUAL "")
    message(STATUS "GIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
    add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
endif()
