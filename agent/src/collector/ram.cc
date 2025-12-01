// agent/src/monitors/ram.cc
#include "monitors/ram.h"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace monitor {

RamStatus RamMonitor::getStatus() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    std::unordered_map<std::string, uint64_t> mem_info;
    
    // 逐行读取并解析 Key-Value
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key;
        uint64_t value;
        std::string unit; // kb

        if (ss >> key >> value) {
            if (!key.empty() && key.back() == ':') {
                key.pop_back(); // remove colon
                mem_info[key] = value;
            }
        }
    }

    RamStatus status{};
    status.total_kb = mem_info["MemTotal"];
    status.available_kb = mem_info["MemAvailable"];
    status.free_kb = mem_info["MemFree"];
    status.swap_total_kb = mem_info["SwapTotal"];
    uint64_t swap_free = mem_info["SwapFree"];
    
    // if no modern kernal available (MemAvailable), use Free + Buffers + Cached
    if (status.available_kb == 0 && mem_info.count("Cached")) {
        status.available_kb = status.free_kb + mem_info["Buffers"] + mem_info["Cached"];
    }

    // used Total - Available
    if (status.total_kb >= status.available_kb) {
        status.used_kb = status.total_kb - status.available_kb;
    }
    
    if (status.swap_total_kb >= swap_free) {
        status.swap_used_kb = status.swap_total_kb - swap_free;
    }

    return status;
}

} // namespace monitor