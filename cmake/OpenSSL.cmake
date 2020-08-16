if (WITH_TLS)
    set(OPENSSL_ROOT_DIR ${XMRIG_DEPS})

    if (WIN32)
        set(OPENSSL_USE_STATIC_LIBS TRUE)
        set(OPENSSL_MSVC_STATIC_RT TRUE)
    elseif (APPLE)
        set(OPENSSL_USE_STATIC_LIBS TRUE)
    endif()

    find_package(OpenSSL)

    if (OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIR})

        if (WIN32)
            list(APPEND OPENSSL_LIBRARIES Crypt32)
        endif()
    else()
        message(FATAL_ERROR "OpenSSL NOT found: use `-DWITH_TLS=OFF` to build without TLS support")
    endif()

    add_definitions(/DXMRIG_FEATURE_TLS)
else()
    set(TLS_SOURCES "")
    set(OPENSSL_LIBRARIES "")
    remove_definitions(/DXMRIG_FEATURE_TLS)
endif()
