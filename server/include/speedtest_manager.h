#pragma once

#include <map>
#include <string>
#include <vector>
#include <ctime>
#include <mutex>
#include "collector.pb.h"

namespace serverstatus {

class SpeedtestManager {
public:
    static SpeedtestManager& Instance() {
        static SpeedtestManager instance;
        return instance;
    }

    // called inside the gRPC handler to get tasks for a specific agent
    std::vector<SpeedtestTask> GetTasksForAgent(const std::string& agent_uuid);

private:
    SpeedtestManager() = default;
    
    // Map<AgentUUID, Map<TargetName, LastExecutionTimestamp>>
    std::map<std::string, std::map<std::string, time_t>> last_execution_time_;
    std::mutex mutex_;
};

}