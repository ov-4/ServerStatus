#include "collector/cpu.h"
#include <fstream>
#include <sstream>
#include <string>

namespace collector {

CpuMonitor::CpuMonitor() : last_total_time_(0), last_idle_time_(0) {
    serverstatus::SystemState dummy;
    Collect(&dummy); 
}

bool CpuMonitor::readStats(CpuStats& stats) {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) return false;

    std::string line;
    if (std::getline(file, line)) {
        if (line.rfind("cpu ", 0) == 0) {
            std::istringstream ss(line);
            std::string label;
            ss >> label 
               >> stats.user >> stats.nice >> stats.system 
               >> stats.idle >> stats.iowait >> stats.irq 
               >> stats.softirq >> stats.steal;
            return true;
        }
    }
    return false;
}

void CpuMonitor::Collect(serverstatus::SystemState* state) {
    CpuStats stats{};
    if (!readStats(stats)) {
        state->set_cpu_usage(0.0);
        return;
    }

    unsigned long long idle_time = stats.idle + stats.iowait;
    unsigned long long active_time = stats.user + stats.nice + stats.system + 
                                     stats.irq + stats.softirq + stats.steal;
    unsigned long long total_time = idle_time + active_time;

    unsigned long long total_delta = total_time - last_total_time_;
    unsigned long long idle_delta = idle_time - last_idle_time_;

    float usage = 0.0;
    if (total_delta > 0) {
        usage = (1.0f - (static_cast<float>(idle_delta) / total_delta)) * 100.0f;
    }

    last_total_time_ = total_time;
    last_idle_time_ = idle_time;

    state->set_cpu_usage(usage);
}

} // namespace collector