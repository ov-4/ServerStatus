#include "speedtest_manager.h"
#include "server_config.h"
#include <iostream>

namespace serverstatus {

std::vector<SpeedtestTask> SpeedtestManager::GetTasksForAgent(const std::string& agent_uuid) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SpeedtestTask> tasks;
    
    time_t now = std::time(nullptr);
    
    // get current shared_ptr
    auto cfg = ServerConfigLoader::Instance().Get();

    if (!cfg->speedtest_enabled) return tasks;

    for (const auto& item : cfg->speed_tests) {
        if (!item.enabled) continue;

        // use item.name
        time_t last_run = last_execution_time_[agent_uuid][item.name];
        
        if (now - last_run >= item.interval) {
            SpeedtestTask task;
            task.set_id(item.name);
            task.set_type(item.method);
            task.set_target(item.target);
            tasks.push_back(task);

            last_execution_time_[agent_uuid][item.name] = now;
            
            // std::cout << "[Scheduler] Sending task '" << item.name << "'..." << std::endl;
        }
    }

    return tasks;
}

}