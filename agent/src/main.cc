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

void print_monitor_info(double cpu, const monitor::RamStatus& ram, 
                        const monitor::DiskStatus& disk, const monitor::NetworkStatus& net) {
    std::cout << "\033[2J\033[1;1H";

    std::cout << "----------------------------------------" << std::endl;
    std::cout << " CPU  : " << std::fixed << std::setprecision(1) << cpu << "%" << std::endl;
    
    std::cout << " RAM  : " << format_bytes(ram.used_kb * 1024) << " / " 
              << format_bytes(ram.total_kb * 1024) << std::endl;
              
    std::cout << " Disk : " << format_bytes(disk.used_bytes) << " / " 
              << format_bytes(disk.total_bytes) << std::endl;
              
    std::cout << " Speed: ↓ " << format_bytes(net.rx_speed) << "/s"
              << "  ↑ " << format_bytes(net.tx_speed) << "/s" << std::endl;
    
    std::cout << " Total: ↓ " << format_bytes(net.total_rx_bytes)
              << "  ↑ " << format_bytes(net.total_tx_bytes) << std::endl;
}

int main() {
    std::cout << "Starting ServerStatus Agent (Full Monitor)..." << std::endl;

    monitor::CpuMonitor cpu_mon;
    monitor::RamMonitor ram_mon;
    monitor::DiskMonitor disk_mon;
    monitor::NetworkMonitor net_mon;

    while (true) {
        double cpu = cpu_mon.getUsage();
        auto ram = ram_mon.getStatus();
        auto disk = disk_mon.getStatus("/");
        auto net = net_mon.getStatus();

        print_monitor_info(cpu, ram, disk, net);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}