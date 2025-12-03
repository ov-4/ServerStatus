cmake_minimum_required(VERSION 3.15)
project(ServerStatusWeb CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

message(STATUS "Fetching nlohmann/json for WebAssembly...")
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(json)

set(WEB_SOURCES
    src/main.cc
    src/app.cc
    src/network.cc
    src/ui_renderer.cc
)

add_executable(server_status_web ${WEB_SOURCES})

target_include_directories(server_status_web PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server_status_web PRIVATE nlohmann_json::nlohmann_json)

# Emscripten 
target_compile_options(server_status_web PRIVATE "-Os") # optimize for size


target_link_options(server_status_web PRIVATE
    "-sWASM=1"                      # produce WASM
    "-sFETCH=1"                     # enable Fetch API
    "-sALLOW_MEMORY_GROWTH=1"
    "-sNO_EXIT_RUNTIME=1"           # main()
    "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']" # allow JS to call CC
    "--bind"                        # enable Embind
)

set_target_properties(server_status_web PROPERTIES 
    OUTPUT_NAME "server_status"
    SUFFIX ".js"
)

add_custom_command(TARGET server_status_web POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:server_status_web>
    ${CMAKE_CURRENT_SOURCE_DIR}/public/server_status.js
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE_DIR:server_status_web>/server_status.wasm
    ${CMAKE_CURRENT_SOURCE_DIR}/public/server_status.wasm
    COMMENT "Copying Wasm/JS to public directory"
)