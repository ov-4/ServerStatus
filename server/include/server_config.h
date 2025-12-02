#pragma once

// #define SERVER_PORT 8080

// #define SERVER_HOST "0.0.0.0"

// #define GPRC_ADDRESS "0.0.0.0:8081"

// #define MAX_HISTORY_SIZE 120


#include <string>
#include <vector>
#include <map>

namespace serverstatus {

struct SpeedtestConfigItem {
    bool enabled;
    std::string method;
    int interval;
    std::string target;
};

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
    bool LoadServers(const std::string& path);
    bool Verify(const std::string& uuid, const std::string& token);

    const ServerConfigData& Get() const { return data_; }
    const std::map<std::string, SpeedtestConfigItem>& GetSpeedtestTargets() const { return speedtest_targets_; }

private:
    ServerConfig() = default;
    ServerConfigData data_;
    std::map<std::string, std::string> authorized_servers_;
    std::map<std::string, SpeedtestConfigItem> speedtest_targets_;
};

} // namespace serverstatus