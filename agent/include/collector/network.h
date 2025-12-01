#pragma once

#include <cstdint>

namespace monitor {

struct NetworkStatus {
    uint64_t total_rx_bytes;
    uint64_t total_tx_bytes;
    uint64_t rx_speed;
    uint64_t tx_speed;
};

class NetworkMonitor {
public:
    NetworkMonitor();
    NetworkStatus getStatus();

private:
    uint64_t last_total_rx_;
    uint64_t last_total_tx_;
    // simple time tracking
    // assume interval of 1s is used
    // more accurate implements maybe added soon?
};

} // namespace monitor