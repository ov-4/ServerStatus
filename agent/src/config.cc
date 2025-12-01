#include "agent_config.h"
#include "default_config.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace collector {

bool AgentConfig::Load(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Agent] Config file not found. Writing default to: " << path << std::endl;
        
        std::ofstream fout(path);
        fout << DEFAULT_CONFIG_CONTENT;
        fout.close();
    }

    try {
        YAML::Node config = YAML::LoadFile(path);
        data_.server_address = config["server_address"].as<std::string>("127.0.0.1:8081");
        std::cout << "[Agent] Loaded config from " << path << ", target: " << data_.server_address << std::endl;
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "[Agent] YAML Error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace collector