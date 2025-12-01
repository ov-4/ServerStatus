cmake_minimum_required(VERSION 3.15)

project(ServerStatus
    VERSION 0.1
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(OpenSSL REQUIRED)

message(STATUS "Fetching nlohmann/json...")
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

option(HTTPLIB_REQUIRE_OPENSSL "Require OpenSSL" ON)

message(STATUS "Fetching cpp-httplib...")
FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.14.3
)
FetchContent_MakeAvailable(cpp-httplib)

add_subdirectory(agent)
# add_subdirectory(server)