#include "agent_config.h"
#include "utils.h"
#include <iostream>
#include <filesystem> // C++17

namespace collector {

bool AgentConfigManager::Init(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cout << "[Agent] Config not found. Generating default..." << std::endl;
        
        AgentConfig default_conf;
        default_conf.server_address = "127.0.0.1:8081";
        default_conf.uuid = Utils::GenerateUUID();
        default_conf.token = Utils::GenerateRandomPassword(32);

        // save
        if (AgentConfigLoader::Instance().Save(path, default_conf)) {
            std::cout << "============================================" << std::endl;
            std::cout << "  UUID : " << default_conf.uuid << std::endl;
            std::cout << "  Token: " << default_conf.token << std::endl;
            std::cout << "============================================" << std::endl;
        } else {
            std::cerr << "[Error] Failed to write config file!" << std::endl;
            return false;
        }
    }

    // load normally
    return AgentConfigLoader::Instance().Load(path);
}

} // namespace collector

namespace YAML {

// save
Node convert<collector::AgentConfig>::encode(const collector::AgentConfig& rhs) {
    Node node;
    node["server_address"] = rhs.server_address;
    node["uuid"] = rhs.uuid;
    node["token"] = rhs.token;
    return node;
}

// load
bool convert<collector::AgentConfig>::decode(const Node& node, collector::AgentConfig& rhs) {
    if (!node.IsMap()) return false;

    rhs.server_address = node["server_address"].as<std::string>("127.0.0.1:8081");
    // if uuid or token is missing, keep it empty
    if (node["uuid"]) rhs.uuid = node["uuid"].as<std::string>();
    if (node["token"]) rhs.token = node["token"].as<std::string>();

    return true;
}

} // namespace YAML