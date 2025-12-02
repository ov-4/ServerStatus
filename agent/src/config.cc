#include "agent_config.h"
#include "default_config.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace collector {

bool AgentConfig::Load(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cout << "[Agent] Config file not found. Generating new identity..." << std::endl;
        
        // gnerate UUID and token
        std::string new_uuid = Utils::GenerateUUID();
        std::string new_token = Utils::GenerateRandomPassword(32);
        
        YAML::Node new_config;
        new_config["server_address"] = "127.0.0.1:8081";
        new_config["uuid"] = new_uuid;
        new_config["token"] = new_token;
        
        std::ofstream fout(path);
        fout << new_config;
        fout.close();
        
        std::cout << "============================================" << std::endl;
        std::cout << "  UUID : " << new_uuid << std::endl;
        std::cout << "  Token: " << new_token << std::endl;
        std::cout << "============================================" << std::endl;
    }

    try {
        YAML::Node config = YAML::LoadFile(path);
        data_.server_address = config["server_address"].as<std::string>("127.0.0.1:8081");
        
        if (config["uuid"]) data_.uuid = config["uuid"].as<std::string>();
        if (config["token"]) data_.token = config["token"].as<std::string>();
        
        if (data_.uuid.empty()) {
            std::cerr << "[Warning] No UUID found" << std::endl;
        }

        std::cout << "[Agent] Loaded config from " << path << std::endl;
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "[Agent] YAML Error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace collector