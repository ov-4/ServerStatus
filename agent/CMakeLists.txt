set(MONITOR_SOURCES
    src/monitors/cpu.cc
    src/monitors/ram.cc
    src/monitors/disk.cc
    src/monitors/network.cc
)

add_library(agent_monitors STATIC ${MONITOR_SOURCES})

target_include_directories(agent_monitors PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(agent_monitors PRIVATE nlohmann_json::nlohmann_json)

add_executable(server_status_agent src/main.cc)

target_link_libraries(server_status_agent PRIVATE agent_monitors OpenSSL::SSL)