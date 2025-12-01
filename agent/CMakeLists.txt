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

target_link_libraries(agent_collector PRIVATE nlohmann_json::nlohmann_json)

add_executable(server_status_agent src/main.cc)

target_link_libraries(server_status_agent PRIVATE agent_collector OpenSSL::SSL)