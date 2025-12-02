#include "server_config.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "default_config.h"

namespace serverstatus {

// config.yaml initializaion
bool ServerConfigManager::InitConfig(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Config] Configuration file not found. Writing default to: " << path << std::endl;
        std::ofstream fout(path);
        if (fout.is_open()) {
            fout << DEFAULT_CONFIG_CONTENT;
            fout.close();
        } else {
            std::cerr << "[Error] Failed to write default config to " << path << std::endl;
            return false;
        }
    }

    return ServerConfigLoader::Instance().Load(path);
}

// server-list.yaml initialization
bool ServerConfigManager::InitServerList(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Config] Server list not found. Creating example: " << path << std::endl;
        std::ofstream fout(path);
        if (fout.is_open()) {
            fout << "servers:\n";
            fout << "  - name: \"Example Server\"\n";
            fout << "    uuid: \"your-uuid-here\"\n";
            fout << "    password: \"your-token-here\"\n";
            fout.close();
        }
    }

    return ServerListLoader::Instance().Load(path);
}

} // namespace serverstatus


namespace YAML {

// SpeedtestItem
bool convert<serverstatus::SpeedtestItem>::decode(const Node& node, serverstatus::SpeedtestItem& rhs) {
    if (!node.IsMap()) return false;
    rhs.name = node["name"].as<std::string>("unknown");
    rhs.enabled = node["enabled"].as<bool>(true);
    rhs.method = node["method"].as<std::string>("tcp");
    rhs.target = node["target"].as<std::string>("");
    rhs.interval = node["interval"].as<int>(60);
    return true;
}

Node convert<serverstatus::SpeedtestItem>::encode(const serverstatus::SpeedtestItem& rhs) {
    Node node;
    node["name"] = rhs.name;
    node["enabled"] = rhs.enabled;
    node["method"] = rhs.method;
    node["target"] = rhs.target;
    node["interval"] = rhs.interval;
    return node;
}

// ServerConfig
bool convert<serverstatus::ServerConfig>::decode(const Node& node, serverstatus::ServerConfig& rhs) {
    if (!node.IsMap()) return false;

    rhs.listen_host = node["listen_host"].as<std::string>("0.0.0.0");
    rhs.listen_port = node["listen_port"].as<int>(8080);
    rhs.grpc_host = node["grpc_host"].as<std::string>("0.0.0.0");
    rhs.grpc_port = node["grpc_port"].as<int>(8081);
    rhs.history_size = node["history_size"].as<int>(120);

    if (node["speedtest"] && node["speedtest"].IsMap()) {
        auto st_node = node["speedtest"];
        rhs.speedtest_enabled = st_node["enabled"].as<bool>(false);
        
        // use vector
        if (st_node["test"] && st_node["test"].IsSequence()) {
            rhs.speed_tests = st_node["test"].as<std::vector<serverstatus::SpeedtestItem>>();
        }
    } else {
        rhs.speedtest_enabled = false;
    }
    return true;
}

// ServerListConfig
bool convert<serverstatus::ServerListConfig>::decode(const Node& node, serverstatus::ServerListConfig& rhs) {
    if (node["servers"] && node["servers"].IsSequence()) {
        for (const auto& item : node["servers"]) {
            std::string uuid = item["uuid"].as<std::string>("");
            std::string pwd = item["password"].as<std::string>("");
            if (!uuid.empty()) {
                rhs.servers[uuid] = pwd;
            }
        }
        return true;
    }
    return false;
}

} // namespace YAML