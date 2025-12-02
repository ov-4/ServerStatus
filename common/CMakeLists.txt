add_library(server_status_common STATIC
    src/utils.cc
)

target_include_directories(server_status_common PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)