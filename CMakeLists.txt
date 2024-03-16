cmake_minimum_required(VERSION 3.5)
project(Raptor VERSION 2024.2.0 HOMEPAGE_URL "https://github.com/InfpHub/Raptor" LANGUAGES CXX)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/mingw_64" CACHE PATH "Qt Installation Path")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_PREFIX_PATH "/opt/Qt/6.8.0/gcc_64" CACHE PATH "Qt Installation Path")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql SvgWidgets Network)
qt_standard_project_setup()

if (NOT Qt6_FOUND)
    message(FATAL_ERROR "Not Found Qt6!")
endif ()

if (QT_VERSION_MAJOR LESS 6)
    message(FATAL_ERROR "${PROJECT_NAME} ${PROJECT_VERSION} Requires Qt 6!")
endif ()

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} Uses Qt ${Qt6Core_VERSION}")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.cpp
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.h
        ${PROJECT_SOURCE_DIR}/Src/Raptor*.ui
)

add_compile_definitions(QT_MESSAGELOGCONTEXT)

configure_file(
        ${PROJECT_SOURCE_DIR}/Src/Common/RaptorDefine.h.in
        ${PROJECT_SOURCE_DIR}/Src/Common/RaptorDefine.h
)

include_directories(
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Curl/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Sass/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/SECP256K1/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/UV/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Yaml/include
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/ZInt/include
)

link_directories(
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Curl/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/OpenSSL/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Sass/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/SECP256K1/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/UV/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/Yaml/lib
        ${PROJECT_SOURCE_DIR}/3rd-Party/${CMAKE_SYSTEM_NAME}/ZInt/lib
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    configure_file(
            ${PROJECT_SOURCE_DIR}/Src/Raptor.rc.in
            ${PROJECT_SOURCE_DIR}/Src/Raptor.rc
    )

    add_executable(
            ${CMAKE_PROJECT_NAME}
            ${PROJECT_SOURCES}
            ${PROJECT_SOURCE_DIR}/Src/Raptor.rc
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_BUILD_RPATH ".")
    set(CMAKE_INSTALL_RPATH ".")
    add_executable(
            ${CMAKE_PROJECT_NAME}
            ${PROJECT_SOURCES}
    )
endif ()

target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::SvgWidgets
        Qt${QT_VERSION_MAJOR}::Network
        debug Curl
        debug SECP256K1
        debug Sass
        debug SSL
        debug UV
        debug Yaml
        debug ZInt
        optimized Curl
        optimized SECP256K1
        optimized Sass
        optimized SSL
        optimized UV
        optimized Yaml
        optimized ZInt
)

set_target_properties(
        ${CMAKE_PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${CMAKE_PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}"
        WIN32_EXECUTABLE TRUE
)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/Store"
)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/Store"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Store" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/Store"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.yaml")
        add_custom_command(
                TARGET ${CMAKE_PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Config/${PROJECT_NAME}.yaml" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.yaml"
        )
    endif ()

    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.db")
        add_custom_command(
                TARGET ${CMAKE_PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Database/${PROJECT_NAME}.db" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.db"
        )
    endif ()

    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(Players "MPV" "VLC" "PotPlayer")
        foreach (Player ${Players})
            add_custom_command(
                    TARGET ${CMAKE_PROJECT_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/Store/${Player}"
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Resource/Player/${Player}" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/Store/${Player}"
            )
        endforeach ()
    endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Config/${PROJECT_NAME}.yaml" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.yaml"
    )

    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Resource/Database/${PROJECT_NAME}.db" "${PROJECT_SOURCE_DIR}/Target/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.db"
    )
endif ()
