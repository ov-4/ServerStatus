#pragma once

#include "collector.pb.h"
#include <cstdint>

namespace collector {

class NetworkMonitor {
public:
    NetworkMonitor();
    void Collect(serverstatus::SystemState* state);

private:
    uint64_t last_total_rx_;
    uint64_t last_total_tx_;
};

} // namespace collector