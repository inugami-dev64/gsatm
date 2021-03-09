cmake_minimum_required(VERSION 3.10)

project(gsatm C)
set(GSATM_TARGET "gsatm")

set (
    GSATM_SOURCES
    converter.c
    fm.c
    hashmap.c
    atm.c
)


add_executable (
    ${GSATM_TARGET}
    ${GSATM_SOURCES}
)

if(CURL_LIBRARY_PATH) 
    target_link_directories (
        ${GSATM_TARGET}
        PUBLIC ${CURL_LIBRARY_PATH}
    )   
endif()

if(CURL_INCLUDE_PATH)
    target_include_directories (
        ${GSATM_TARGET}
        PUBLIC ${CURL_INCLUDE_PATH}
    )
endif()

target_link_libraries (
    ${GSATM_TARGET}
    curl
)

file (
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/cash.info
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
