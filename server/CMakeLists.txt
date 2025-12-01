set(SERVER_SOURCES
    src/main.cc
    src/handlers/update_handler.cc
)

add_executable(server_status_server ${SERVER_SOURCES})

target_include_directories(server_status_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${cpp-httplib_SOURCE_DIR}
)

find_package(Threads REQUIRED)

target_link_libraries(server_status_server PRIVATE 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    Threads::Threads
)