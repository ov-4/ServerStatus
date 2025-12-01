#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include "service.h"
#include "server_config.h"
#include "storage.h"

using grpc::Server;
using grpc::ServerBuilder;

void RunHttpServer() {
    httplib::Server svr;

    // API
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        auto stats_list = serverstatus::Storage::Instance().GetAllAsJson();
        nlohmann::json json_response = stats_list;
        
        res.set_header("Access-Control-Allow-Origin", "*"); // allow CORS
        res.set_content(json_response.dump(), "application/json");
    });

    // index
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        // 尝试读取 web/index.html
        std::ifstream file("web/index.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("web/index.html not found. Please ensure you are running from the correct directory.", "text/plain");
        }
    });

    std::cout << "HTTP Server listening on " << SERVER_HOST << ":" << SERVER_PORT << std::endl;
    svr.listen(SERVER_HOST, SERVER_PORT);
}

int main() {
    std::thread http_thread(RunHttpServer);
    http_thread.detach(); // seperate thread for HTTP server

    std::string server_address(GPRC_ADDRESS);
    serverstatus::MonitorServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "gRPC Server listening on " << server_address << std::endl;

    server->Wait();

    return 0;
}