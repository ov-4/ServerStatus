#include "server_config.h"
#include "default_config.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace serverstatus {

bool ServerConfig::Load(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Config] Configuration file not found. Writing default to: " << path << std::endl;
        
        std::ofstream fout(path);
        fout << DEFAULT_CONFIG_CONTENT;
        fout.close();
    }

    try {
        YAML::Node config = YAML::LoadFile(path);
        
        data_.listen_host = config["listen_host"].as<std::string>("0.0.0.0");
        data_.listen_port = config["listen_port"].as<int>(8080);
        data_.grpc_host = config["grpc_host"].as<std::string>("0.0.0.0");
        data_.grpc_port = config["grpc_port"].as<int>(8081);
        data_.history_size = config["history_size"].as<int>(120);
        
        std::cout << "[Config] Loaded configuration from " << path << std::endl;
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "[Config] Error parsing YAML: " << e.what() << std::endl;
        return false;
    }
}

bool ServerConfig::LoadServers(const std::string& path) { // server list
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Config] Server list not found. Creating example: " << path << std::endl;
        std::ofstream fout(path);
        fout << "servers:\n";
        fout << "  - name: \"Example Server\"\n";
        fout << "    uuid: \"your-uuid-here\"\n";
        fout << "    password: \"your-token-here\"\n";
        fout.close();
        return true;
    }

    try {
        YAML::Node config = YAML::LoadFile(path);
        if (config["servers"] && config["servers"].IsSequence()) {
            for (const auto& node : config["servers"]) {
                std::string uuid = node["uuid"].as<std::string>("");
                std::string pwd = node["password"].as<std::string>("");
                if (!uuid.empty()) {
                    authorized_servers_[uuid] = pwd;
                }
            }
        }
        std::cout << "[Config] Loaded " << authorized_servers_.size() << " servers from whitelist." << std::endl;
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "[Config] Error parsing server list: " << e.what() << std::endl;
        return false;
    }
}

bool ServerConfig::Verify(const std::string& uuid, const std::string& token) {
    auto it = authorized_servers_.find(uuid);
    if (it == authorized_servers_.end()) {
        return false; // UUID not found
    }
    return it->second == token; // Check token
}

} // namespace serverstatus