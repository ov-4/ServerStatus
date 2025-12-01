#include "monitors/network.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace monitor {

NetworkMonitor::NetworkMonitor() : last_total_rx_(0), last_total_tx_(0) {
    getStatus();
}

// 方法实现
NetworkStatus NetworkMonitor::getStatus() {
    std::ifstream file("/proc/net/dev");
    std::string line;
    uint64_t current_total_rx = 0;
    uint64_t current_total_tx = 0;

    if (file.is_open()) {
        std::getline(file, line); // skip header line 1
        std::getline(file, line); 

        while (std::getline(file, line)) {
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;

            line[colon_pos] = ' '; // handle "eth0:123"
            
            std::istringstream ss(line);
            std::string iface_name;
            uint64_t rx_bytes = 0, tx_bytes = 0, dummy = 0;

            ss >> iface_name;
            if (iface_name == "lo") continue; // skip loopback

            ss >> rx_bytes;
            for (int i = 0; i < 7; ++i) ss >> dummy;
            ss >> tx_bytes;

            current_total_rx += rx_bytes;
            current_total_tx += tx_bytes;
        }
    }

    NetworkStatus status{};
    status.total_rx_bytes = current_total_rx;
    status.total_tx_bytes = current_total_tx;

    if (last_total_rx_ != 0 && current_total_rx >= last_total_rx_) {
        status.rx_speed = current_total_rx - last_total_rx_;
    } else {
        status.rx_speed = 0;
    }

    if (last_total_tx_ != 0 && current_total_tx >= last_total_tx_) {
        status.tx_speed = current_total_tx - last_total_tx_;
    } else {
        status.tx_speed = 0;
    }

    last_total_rx_ = current_total_rx;
    last_total_tx_ = current_total_tx;

    return status;
}

} // namespace monitor