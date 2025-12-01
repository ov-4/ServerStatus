#include "collector/network.h"
#include <fstream>
#include <sstream>
#include <string>

namespace monitor {

NetworkMonitor::NetworkMonitor() : last_total_rx_(0), last_total_tx_(0) {
    serverstatus::SystemState dummy;
    Collect(&dummy);
}

void NetworkMonitor::Collect(serverstatus::SystemState* state) {
    std::ifstream file("/proc/net/dev");
    std::string line;
    uint64_t current_total_rx = 0;
    uint64_t current_total_tx = 0;

    if (file.is_open()) {
        std::getline(file, line); 
        std::getline(file, line); 

        while (std::getline(file, line)) {
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;

            line[colon_pos] = ' '; 
            
            std::istringstream ss(line);
            std::string iface_name;
            uint64_t rx_bytes = 0, tx_bytes = 0, dummy_val = 0;

            ss >> iface_name;
            if (iface_name == "lo") continue;

            ss >> rx_bytes;
            for (int i = 0; i < 7; ++i) ss >> dummy_val;
            ss >> tx_bytes;

            current_total_rx += rx_bytes;
            current_total_tx += tx_bytes;
        }
    }

    uint64_t rx_speed = 0;
    uint64_t tx_speed = 0;

    if (last_total_rx_ != 0 && current_total_rx >= last_total_rx_) {
        rx_speed = current_total_rx - last_total_rx_;
    }
    
    if (last_total_tx_ != 0 && current_total_tx >= last_total_tx_) {
        tx_speed = current_total_tx - last_total_tx_;
    }

    last_total_rx_ = current_total_rx;
    last_total_tx_ = current_total_tx;

    state->set_network_rx_speed(rx_speed);
    state->set_network_tx_speed(tx_speed);
}

} // namespace monitor