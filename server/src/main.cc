#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include <httplib.h>

#include "service.h"
#include "server_config.h"
#include "api/stats_handler.h"
#include "api/history_handler.h"

using grpc::Server;
using grpc::ServerBuilder;

void RunHttpServer() {
    httplib::Server svr;
    
    auto config = serverstatus::ServerConfigLoader::Instance().Get();

    // api routes
    svr.Get("/api/stats", api::StatsHandler::Handle);
    svr.Get("/api/history", api::HistoryHandler::Handle);

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
    
    svr.set_mount_point("/assets", "web/assets");

    std::cout << "[HTTP] Server listening on " << config->listen_host << ":" << config->listen_port << std::endl;
    svr.listen(config->listen_host.c_str(), config->listen_port);
}

int main() {
    if (!serverstatus::ServerConfigManager::InitConfig("config.yaml")) {
        std::cerr << "Failed to load config.yaml" << std::endl;
        return 1;
    }
    
    if (!serverstatus::ServerConfigManager::InitServerList("server-list.yaml")) {
        std::cerr << "Failed to load server-list.yaml" << std::endl;
    } else {
        auto list_cfg = serverstatus::ServerListLoader::Instance().Get();
        std::cout << "[Config] Loaded " << list_cfg->servers.size() << " servers from whitelist." << std::endl;
    }

    auto config = serverstatus::ServerConfigLoader::Instance().Get();

    std::thread http_thread(RunHttpServer);
    http_thread.detach(); 

    std::string server_address = config->grpc_host + ":" + std::to_string(config->grpc_port);
    serverstatus::MonitorServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "[gRPC] Server listening on " << server_address << std::endl;

    server->Wait();

    return 0;
}