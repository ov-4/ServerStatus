#include "collector/ram.h"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace collector {

void RamMonitor::Collect(serverstatus::SystemState* state) {
    std::ifstream file("/proc/meminfo");
    std::string line;
    std::unordered_map<std::string, uint64_t> mem_info;
    
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key;
        uint64_t value;
        std::string unit; 

        if (ss >> key >> value) {
            if (!key.empty() && key.back() == ':') {
                key.pop_back();
                mem_info[key] = value;
            }
        }
    }

    uint64_t total_kb = mem_info["MemTotal"];
    uint64_t available_kb = mem_info["MemAvailable"];
    uint64_t free_kb = mem_info["MemFree"];

    if (available_kb == 0 && mem_info.count("Cached")) {
        available_kb = free_kb + mem_info["Buffers"] + mem_info["Cached"];
    }

    uint64_t used_kb = 0;
    if (total_kb >= available_kb) {
        used_kb = total_kb - available_kb;
    }

    state->set_memory_total(total_kb * 1024);
    state->set_memory_used(used_kb * 1024);
}

} // namespace collector