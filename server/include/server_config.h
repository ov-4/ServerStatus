#pragma once

// #define SERVER_PORT 8080

// #define SERVER_HOST "0.0.0.0"

// #define GPRC_ADDRESS "0.0.0.0:8081"

// #define MAX_HISTORY_SIZE 120


#include <string>
#include <memory>

namespace serverstatus {

struct ServerConfigData {
    std::string listen_host;
    int listen_port;
    std::string grpc_host;
    int grpc_port;
    int history_size;
};

class ServerConfig {
public:
    static ServerConfig& Instance() {
        static ServerConfig instance;
        return instance;
    }

    bool Load(const std::string& path);

    const ServerConfigData& Get() const { return data_; }

private:
    ServerConfig() = default;
    ServerConfigData data_;
};

} // namespace serverstatus