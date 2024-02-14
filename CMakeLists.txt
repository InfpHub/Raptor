cmake_minimum_required(VERSION 3.5)
project(Raptor VERSION 2024.1.0 HOMEPAGE_URL "https://github.com/InfpPointPlus/Raptor" LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "D:/Environment/Share/Qt-5.15.10-MSVC-2019-32Bit" CACHE PATH "Qt Installation Path")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql Svg Network)

if (NOT Qt5_FOUND)
    message(FATAL_ERROR "Not Found Qt5!")
endif ()

if (Qt5Core_VERSION_MINOR LESS 15)
    message(FATAL_ERROR "${PROJECT_NAME} ${PROJECT_VERSION} Requires Qt ${Qt5Core_VERSION_MAJOR}.15.X")
endif ()

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} Uses Qt: ${Qt5Core_VERSION}")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(Architecture x64)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(Architecture x86)
else ()
    message(FATAL_ERROR "Unknown architecture!")
endif ()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.cpp
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.h
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.ui
)

option(Standalone "Static link ${PROJECT_NAME}" OFF)
set(PROJECT_COMPILE_DEFINITION QT_MESSAGELOGCONTEXT)
set(CMAKE_EXE_LINKER_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS")

if (Standalone)
    set(Runtime "Static")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
    list(APPEND PROJECT_COMPILE_DEFINITION CURL_STATICLIB)
    list(APPEND PROJECT_COMPILE_DEFINITION STANDALONE)
else ()
    set(Runtime "Share")
    list(APPEND PROJECT_COMPILE_DEFINITION INTEGRATED)
endif ()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_definitions(${PROJECT_COMPILE_DEFINITION})

configure_file(
        ${PROJECT_SOURCE_DIR}/Src/Common/RaptorDefine.h.in
        ${PROJECT_SOURCE_DIR}/Src/Common/RaptorDefine.h
)

include_directories(
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Curl/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Crypto++/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/OpenSSL/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Sass/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/SECP256K1/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/ZInt/include
)

link_directories(
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Curl/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Crypto++/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/OpenSSL/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/Sass/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/SECP256K1/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${Architecture}/${Runtime}/ZInt/lib
)

configure_file(
        ${PROJECT_SOURCE_DIR}/Src/Raptor.rc.in
        ${PROJECT_SOURCE_DIR}/Src/Raptor.rc
)

add_executable(
        ${CMAKE_PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_SOURCE_DIR}/Src/Raptor.rc
)

if (Standalone)
    target_link_libraries(
            ${CMAKE_PROJECT_NAME}
            PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Svg
            Qt${QT_VERSION_MAJOR}::Network
            debug Curld
            debug Cryptod
            debug Crypto++d
            debug ZIntd
            debug Sassd
            debug SSLd
            debug wldap32
            optimized Curl
            optimized Crypto
            optimized Crypto++
            optimized ZInt
            optimized Sass
            optimized SSL
            optimized wldap32
    )
else ()
    target_link_libraries(
            ${CMAKE_PROJECT_NAME}
            PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Svg
            Qt${QT_VERSION_MAJOR}::Network
            debug Curl
            debug SECP256K1d
            debug Sassd
            debug ZIntd
            optimized Curl
            optimized SECP256K1
            optimized Sass
            optimized ZInt
    )
endif ()

set_target_properties(
        ${CMAKE_PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${CMAKE_PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}"
        WIN32_EXECUTABLE TRUE
)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Store"
)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/Store"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Store" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Store"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.ini")
        add_custom_command(
                TARGET ${CMAKE_PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Config/${Architecture}/Raptor.ini" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.ini"
        )
    endif ()

    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.db")
        add_custom_command(
                TARGET ${CMAKE_PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Database/Raptor.db" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.db"
        )
    endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Config/${Architecture}/Raptor.ini" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.ini"
    )

    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Database/Raptor.db" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Raptor.db"
    )
endif ()

set(Players "MPV" "VLC" "PotPlayer")
foreach (Player ${Players})
    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/Store/${Player}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Resource/Player/${Architecture}/${Player}" "${PROJECT_SOURCE_DIR}/Target/${Architecture}/${CMAKE_BUILD_TYPE}/${Runtime}/Store/${Player}"
    )
endforeach ()