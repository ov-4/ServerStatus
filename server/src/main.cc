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

    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        auto stats_list = serverstatus::Storage::Instance().GetAllAsJson();
        nlohmann::json json_response = stats_list;
        
        res.set_header("Access-Control-Allow-Origin", "*"); 
        res.set_content(json_response.dump(), "application/json");
    });

    // usage: /api/history?id=127.0.0.1:1234
    svr.Get("/api/history", [](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id")) {
            std::string id = req.get_param_value("id");
            auto history = serverstatus::Storage::Instance().GetHistoryAsJson(id);
            
            if (history.empty()) {
                res.status = 404;
                res.set_content("ID not found or no data", "text/plain");
            } else {
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_content(history.dump(), "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("Missing 'id' parameter", "text/plain");
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
            res.set_content("web/index.html not found. Please ensure you are running from the correct directory.", "text/plain");
        }
    });

    std::cout << "HTTP Server listening on " << SERVER_HOST << ":" << SERVER_PORT << std::endl;
    svr.listen(SERVER_HOST, SERVER_PORT);
}

int main() {
    std::thread http_thread(RunHttpServer);
    http_thread.detach(); 

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