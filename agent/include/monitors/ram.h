#pragma once

#include <cstdint>

namespace monitor {

struct RamStatus {
    uint64_t total_kb;
    uint64_t used_kb;
    uint64_t free_kb;
    uint64_t available_kb;
    uint64_t swap_total_kb;
    uint64_t swap_used_kb;
};

class RamMonitor {
public:
    RamStatus getStatus();
};

}