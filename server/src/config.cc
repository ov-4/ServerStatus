#include "server_config.h"
#include "default_config.h"
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

} // namespace serverstatus