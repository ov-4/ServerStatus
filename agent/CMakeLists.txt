set(COLLECTOR_SOURCES
    src/collector/cpu.cc
    src/collector/ram.cc
    src/collector/disk.cc
    src/collector/network.cc
)

add_library(agent_collector STATIC ${COLLECTOR_SOURCES})

target_include_directories(agent_collector PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(agent_collector PUBLIC 
    nlohmann_json::nlohmann_json
    collector_grpc_proto
)

add_executable(server_status_agent 
    src/main.cc 
    src/client.cc 
)

target_link_libraries(server_status_agent PRIVATE agent_collector OpenSSL::SSL)