#include "agent_config.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace collector {

bool AgentConfig::Load(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Agent] Config file not found. Generating default: " << path << std::endl;
        
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "server_address" << YAML::Value << "127.0.0.1:8081";
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
        fout.close();

        data_.server_address = "127.0.0.1:8081";
        return true;
    }

    try {
        YAML::Node config = YAML::LoadFile(path);
        data_.server_address = config["server_address"].as<std::string>("127.0.0.1:8081");
        std::cout << "[Agent] Loaded config, target: " << data_.server_address << std::endl;
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "[Agent] YAML Error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace collector