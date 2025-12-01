#pragma once
#include <string>

namespace collector {

struct AgentConfigData {
    std::string server_address;
};

class AgentConfig {
public:
    static AgentConfig& Instance() {
        static AgentConfig instance;
        return instance;
    }

    bool Load(const std::string& path);
    const AgentConfigData& Get() const { return data_; }

private:
    AgentConfigData data_;
};

} // namespace collector