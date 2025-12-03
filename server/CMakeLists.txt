set(SERVER_RES_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/resources/config.yaml")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SERVER_RES_CONFIG}")

if(EXISTS "${SERVER_RES_CONFIG}")
    file(READ "${SERVER_RES_CONFIG}" SERVER_YAML_CONTENT)

    # for CC format
    string(REPLACE "\\" "\\\\" SERVER_YAML_CONTENT "${SERVER_YAML_CONTENT}")
    string(REPLACE "\"" "\\\"" SERVER_YAML_CONTENT "${SERVER_YAML_CONTENT}")
    string(REPLACE "\n" "\\n" SERVER_YAML_CONTENT "${SERVER_YAML_CONTENT}")
else()
    message(WARNING "server/resources/config.yaml not found!")
    set(SERVER_YAML_CONTENT "")
endif()

# generate .h file
set(SERVER_GEN_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${SERVER_GEN_HEADER_DIR}")
file(WRITE "${SERVER_GEN_HEADER_DIR}/default_config.h" 
"#pragma once
namespace serverstatus {
    const char* DEFAULT_CONFIG_CONTENT = \"${SERVER_YAML_CONTENT}\";
}
")

# Added the new API handlers here
set(SERVER_SOURCES
    src/main.cc
    src/handlers/update_handler.cc
    src/api/stats_handler.cc
    src/api/history_handler.cc
    src/service.cc
    src/config.cc
    src/speedtest_manager.cc
)

add_executable(server_status_server ${SERVER_SOURCES})

target_include_directories(server_status_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${cpp-httplib_SOURCE_DIR}
    ${SERVER_GEN_HEADER_DIR} # default .yaml file
)

find_package(Threads REQUIRED)

target_link_libraries(server_status_server PRIVATE 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    Threads::Threads
    collector_grpc_proto
    gRPC::grpc++
    yaml-cpp
    server_status_common
)

add_custom_command(TARGET server_status_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:server_status_server>/web
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/web/public
    $<TARGET_FILE_DIR:server_status_server>/web
    COMMENT "Copying ALL web assets (HTML/CSS/WASM) to output directory..."
)