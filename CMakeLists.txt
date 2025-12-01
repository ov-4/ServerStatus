cmake_minimum_required(VERSION 3.15)

project(ServerStatus
    VERSION 0.1
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

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

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})


set(PROTO_FILES ${PROTO_SRC_DIR}/collector.proto)

add_custom_command(
    OUTPUT "${PROTO_BINARY_DIR}/collector.pb.cc"
           "${PROTO_BINARY_DIR}/collector.pb.h"
           "${PROTO_BINARY_DIR}/collector.grpc.pb.cc"
           "${PROTO_BINARY_DIR}/collector.grpc.pb.h"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out="${PROTO_BINARY_DIR}"
         --cpp_out="${PROTO_BINARY_DIR}"
         --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
         -I "${PROTO_SRC_DIR}"
         "${PROTO_FILES}"
    DEPENDS "${PROTO_FILES}"
)

add_library(collector_grpc_proto STATIC
    "${PROTO_BINARY_DIR}/collector.pb.cc"
    "${PROTO_BINARY_DIR}/collector.grpc.pb.cc"
)

target_include_directories(collector_grpc_proto PUBLIC ${PROTO_BINARY_DIR})

target_link_libraries(collector_grpc_proto PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

add_subdirectory(agent)
add_subdirectory(server)