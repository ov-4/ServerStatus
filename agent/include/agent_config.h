#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include "config_loader.h"

namespace collector {

struct AgentConfig {
    std::string server_address;
    std::string uuid;
    std::string token;
};

using AgentConfigLoader = ConfigLoader<AgentConfig>;

class AgentConfigManager {
public:
    static bool Init(const std::string& path);
};

} // namespace collector

namespace YAML {
    template<>
    struct convert<collector::AgentConfig> {
        static Node encode(const collector::AgentConfig& rhs);
        static bool decode(const Node& node, collector::AgentConfig& rhs);
    };
}