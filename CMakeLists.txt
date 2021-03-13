cmake_minimum_required(VERSION 3.12.0)

if(GSATM_BUILD_GUI)
    project(gsatm C CSharp)
else()
    project(gsatm C)
endif()

set(ATM_LIB_TARGET "libatm")
set(ATM_CLI_TARGET "atm_cli")
set(ATM_GUI_TARGET "atm_gui")

set (
    ATM_SOURCES
    algorithm.c
    converter.c
    fm.c
    hashmap.c
    strparse.c
)


if(WIN32)
    # Create dynamic library for GUI
    add_library (
        ${ATM_LIB_TARGET} SHARED
        ${ATM_SOURCES}
    )

    target_include_directories (
        ${ATM_LIB_TARGET}
        PUBLIC ${CMAKE_BINARY_DIR}/curl/include
        PUBLIC ${CMAKE_SOURCE_DIR}
    )

    target_link_directories (
        ${ATM_LIB_TARGET}
        PUBLIC ${CMAKE_BINARY_DIR}/curl
    )

    target_link_libraries (
        ${ATM_LIB_TARGET}
        PUBLIC libcurl_a
        PUBLIC crypt32
        PUBLIC ws2_32
        PUBLIC wldap32
        PUBLIC normaliz
    )

    # Create cli tool
    add_executable (
        ${ATM_CLI_TARGET}
        atm.c
    )

    file (
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/cash.info
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug
    )

    file (
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/cash.info
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release
    )

elseif(LINUX)
    # In Linux create just the executable
    add_executable (
        ${ATM_CLI_TARGET} 
        ${ATM_SOURCES}
        atm.c
    )

    target_link_libraries (
        ${ATM_CLI_TARGET} 
        PUBLIC libcurl
    )
endif()