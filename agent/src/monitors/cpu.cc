#include "monitors/cpu.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>

namespace monitor {

CpuMonitor::CpuMonitor() : last_total_time_(0), last_idle_time_(0) {
    getUsage();
}

bool CpuMonitor::readStats(CpuStats& stats) {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) return false;

    std::string line;
    if (std::getline(file, line)) {
        if (line.rfind("cpu ", 0) == 0) { // starts with "cpu "
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

double CpuMonitor::getUsage() {
    CpuStats stats{};
    if (!readStats(stats)) return 0.0;

    // total CPU time
    // user + nice + system + idle + iowait + irq + softirq + steal
    unsigned long long idle_time = stats.idle + stats.iowait;
    unsigned long long active_time = stats.user + stats.nice + stats.system + 
                                     stats.irq + stats.softirq + stats.steal;
    unsigned long long total_time = idle_time + active_time;

    // differences since last call
    unsigned long long total_delta = total_time - last_total_time_;
    unsigned long long idle_delta = idle_time - last_idle_time_;

    double usage = 0.0;
    if (total_delta > 0) {
        usage = (1.0 - (static_cast<double>(idle_delta) / total_delta)) * 100.0;
    }

    // update status
    last_total_time_ = total_time;
    last_idle_time_ = idle_time;

    return usage;
}

}