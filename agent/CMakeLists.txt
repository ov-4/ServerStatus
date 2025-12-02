set(AGENT_RES_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/resources/config.yaml")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${AGENT_RES_CONFIG}")

if(EXISTS "${AGENT_RES_CONFIG}")
    file(READ "${AGENT_RES_CONFIG}" AGENT_YAML_CONTENT)

    # for CC format
    string(REPLACE "\\" "\\\\" AGENT_YAML_CONTENT "${AGENT_YAML_CONTENT}")
    string(REPLACE "\"" "\\\"" AGENT_YAML_CONTENT "${AGENT_YAML_CONTENT}")
    string(REPLACE "\n" "\\n" AGENT_YAML_CONTENT "${AGENT_YAML_CONTENT}")
else()
    message(WARNING "agent/resources/config.yaml not found! Default config will be empty.")
    set(AGENT_YAML_CONTENT "")
endif()

# generate .h file
set(AGENT_GEN_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${AGENT_GEN_HEADER_DIR}")
file(WRITE "${AGENT_GEN_HEADER_DIR}/default_config.h" 
"#pragma once
namespace collector {
    const char* DEFAULT_CONFIG_CONTENT = \"${AGENT_YAML_CONTENT}\";
}
")


set(COLLECTOR_SOURCES
    src/collector/cpu.cc
    src/collector/ram.cc
    src/collector/disk.cc
    src/collector/network.cc
    src/config.cc
    src/collector/speedtest.cc
)

add_library(agent_collector STATIC ${COLLECTOR_SOURCES})

target_include_directories(agent_collector PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${AGENT_GEN_HEADER_DIR} # default .yaml file
    ${cpp-httplib_SOURCE_DIR}
)

target_link_libraries(agent_collector PUBLIC 
    nlohmann_json::nlohmann_json
    collector_grpc_proto
    yaml-cpp
    server_status_common
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_executable(server_status_agent 
    src/main.cc 
    src/client.cc 
    src/config.cc
)

target_link_libraries(server_status_agent PRIVATE 
    agent_collector 
    OpenSSL::SSL
    server_status_common
)