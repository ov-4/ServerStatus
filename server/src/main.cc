#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "service.h"
#include "server_config.h"

using grpc::Server;
using grpc::ServerBuilder;

int main() {
    std::string server_address(GPRC_ADDRESS);
    serverstatus::MonitorServiceImpl service;

    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ServerStatus gRPC Server listening on " << server_address << std::endl;

    server->Wait();

    return 0;
}