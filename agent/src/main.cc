#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>

#include "collector/cpu.h"
#include "collector/ram.h"
#include "collector/disk.h"
#include "collector/network.h"
#include "collector.pb.h"

std::string format_bytes(uint64_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int i = 0;
    double dblBytes = static_cast<double>(bytes);

    while (dblBytes >= 1024.0 && i < 5) {
        dblBytes /= 1024.0;
        i++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dblBytes << " " << suffixes[i];
    return ss.str();
}

void print_monitor_info(const serverstatus::SystemState& state) {
    std::cout << "\033[2J\033[1;1H";

    std::cout << "----------------------------------------" << std::endl;

    std::cout << " CPU  : " << std::fixed << std::setprecision(1) << state.cpu_usage() << "%" << std::endl;
    
    std::cout << " RAM  : " << format_bytes(state.memory_used()) << " / " 
              << format_bytes(state.memory_total()) << std::endl;
              
    std::cout << " Disk : " << format_bytes(state.disk_used()) << " / " 
              << format_bytes(state.disk_total()) << std::endl;
              
    std::cout << " Speed: ↓ " << format_bytes(state.network_rx_speed()) << "/s"
              << "  ↑ " << format_bytes(state.network_tx_speed()) << "/s" << std::endl;
    
    // std::cout << " Uptime: " << state.uptime() << std::endl;
}

int main() {
    std::cout << "Starting ServerStatus Agent (Protobuf Mode)..." << std::endl;

    monitor::CpuMonitor cpu_mon;
    monitor::RamMonitor ram_mon;
    monitor::DiskMonitor disk_mon;
    monitor::NetworkMonitor net_mon;

    while (true) {
        serverstatus::SystemState state;

        cpu_mon.Collect(&state);
        ram_mon.Collect(&state);
        disk_mon.Collect(&state, "/");
        net_mon.Collect(&state);

        print_monitor_info(state);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}