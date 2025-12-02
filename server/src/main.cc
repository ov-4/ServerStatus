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
    const auto& config = serverstatus::ServerConfig::Instance().Get();

    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        auto stats_list = serverstatus::Storage::Instance().GetAllAsJson();
        nlohmann::json json_response = stats_list;
        
        res.set_header("Access-Control-Allow-Origin", "*"); 
        res.set_content(json_response.dump(), "application/json");
    });

    svr.Get("/api/history", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (req.has_param("id")) {
            std::string id = req.get_param_value("id");
            auto history = serverstatus::Storage::Instance().GetHistoryAsJson(id);
            res.set_content(history.dump(), "application/json");
        } else {
            res.status = 400;
            res.set_content("Missing id param", "text/plain");
        }
    });

    // index
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::ifstream file("web/index.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("web/index.html not found.", "text/plain");
        }
    });

    std::cout << "HTTP Server listening on " << config.listen_host << ":" << config.listen_port << std::endl;
    svr.listen(config.listen_host.c_str(), config.listen_port);
}

int main() {
    if (!serverstatus::ServerConfig::Instance().Load("config.yaml")) {
        std::cerr << "Failed to load config.yaml" << std::endl;
        return 1;
    }
    if (!serverstatus::ServerConfig::Instance().LoadServers("server-list.yaml")) {
        std::cerr << "Failed to load server-list.yaml" << std::endl;
        return 1;
    }

    const auto& config = serverstatus::ServerConfig::Instance().Get();

    std::thread http_thread(RunHttpServer);
    http_thread.detach(); 

    std::string server_address = config.grpc_host + ":" + std::to_string(config.grpc_port);
    serverstatus::MonitorServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "gRPC Server listening on " << server_address << std::endl;

    server->Wait();

    return 0;
}