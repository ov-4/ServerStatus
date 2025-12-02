#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "config_loader.h"

namespace serverstatus {

// ==================== 业务结构体定义 ====================

struct SpeedtestItem {
    std::string name;
    bool enabled;
    std::string method;
    std::string target;
    int interval;
};

struct ServerConfig {
    std::string listen_host;
    int listen_port;
    std::string grpc_host;
    int grpc_port;
    int history_size;

    bool speedtest_enabled;
    std::vector<SpeedtestItem> speed_tests;
};

struct ServerListConfig {
    std::unordered_map<std::string, std::string> servers;

    bool Verify(const std::string& uuid, const std::string& token) const {
        auto it = servers.find(uuid);
        if (it == servers.end()) return false;
        return it->second == token;
    }
};

class ServerConfigManager {
public:

    static bool InitConfig(const std::string& path);

    static bool InitServerList(const std::string& path);
};


using ServerConfigLoader = ConfigLoader<ServerConfig>;
using ServerListLoader = ConfigLoader<ServerListConfig>;

} // namespace serverstatus


namespace YAML {
    // 1. SpeedtestItem
    template<>
    struct convert<serverstatus::SpeedtestItem> {
        static Node encode(const serverstatus::SpeedtestItem& rhs);
        static bool decode(const Node& node, serverstatus::SpeedtestItem& rhs);
    };

    // 2. ServerConfig
    template<>
    struct convert<serverstatus::ServerConfig> {
        static bool decode(const Node& node, serverstatus::ServerConfig& rhs);
    };

    // 3. ServerListConfig
    template<>
    struct convert<serverstatus::ServerListConfig> {
        static bool decode(const Node& node, serverstatus::ServerListConfig& rhs);
    };
}