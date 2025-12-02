#include "speedtest_manager.h"
#include "server_config.h"
#include <iostream>

namespace serverstatus {

std::vector<SpeedtestTask> SpeedtestManager::GetTasksForAgent(const std::string& agent_uuid) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SpeedtestTask> tasks;
    
    time_t now = std::time(nullptr);
    const auto& targets = ServerConfig::Instance().GetSpeedtestTargets();

    for (const auto& [name, config] : targets) {
        if (!config.enabled) continue;

        time_t last_run = last_execution_time_[agent_uuid][name]; // defaults to 0
        
        if (now - last_run >= config.interval) {
            SpeedtestTask task;
            task.set_id(name);
            task.set_type(config.method);
            task.set_target(config.target);
            tasks.push_back(task);

            // up execution time immediately to prevent duplicate scheduling
            last_execution_time_[agent_uuid][name] = now;
            
            std::cout << "[Scheduler] Sending task '" << name << "' to " << agent_uuid << std::endl;
        }
    }

    return tasks;
}

}