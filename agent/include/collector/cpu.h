#pragma once

#include "collector.pb.h"

namespace collector {

class CpuMonitor {
public:
    CpuMonitor();
    
    void Collect(serverstatus::SystemState* state);

private:
    struct CpuStats {
        unsigned long long user;
        unsigned long long nice;
        unsigned long long system;
        unsigned long long idle;
        unsigned long long iowait;
        unsigned long long irq;
        unsigned long long softirq;
        unsigned long long steal;
    };

    bool readStats(CpuStats& stats);
    
    unsigned long long last_total_time_;
    unsigned long long last_idle_time_;
};

} // namespace collector