#pragma once

#include "collector.pb.h"
#include <cstdint>

namespace monitor {

class NetworkMonitor {
public:
    NetworkMonitor();
    void Collect(serverstatus::SystemState* state);

private:
    uint64_t last_total_rx_;
    uint64_t last_total_tx_;
};

} // namespace monitor