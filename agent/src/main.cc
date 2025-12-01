#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

#include "monitors/cpu.h"
#include "monitors/ram.h"

void print_debug_info(double cpu_usage, const monitor::RamStatus& ram) {
    // std::cout << "\033[2J\033[1;1H"; 
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "[System Monitor Debug]" << std::endl;
    
    // CPU
    std::cout << "CPU Usage : " 
              << std::fixed << std::setprecision(2) << cpu_usage << "%" << std::endl;

    // RAM, to MB
    double total_mb = ram.total_kb / 1024.0;
    double used_mb = ram.used_kb / 1024.0;
    double swap_used_mb = ram.swap_used_kb / 1024.0;
    double swap_total_mb = ram.swap_total_kb / 1024.0;

    std::cout << "RAM Usage : " 
              << std::fixed << std::setprecision(2) << used_mb << " / " << total_mb << " MB "
              << "(" << (used_mb/total_mb * 100.0) << "%)" << std::endl;
              
    std::cout << "Swap Usage: " 
              << std::fixed << std::setprecision(2) << swap_used_mb << " / " << swap_total_mb << " MB" << std::endl;
}

int main() {
    std::cout << "Starting ServerStatus Agent..." << std::endl;

    monitor::CpuMonitor cpu_monitor;
    monitor::RamMonitor ram_monitor;

    while (true) {
        double cpu = cpu_monitor.getUsage();
        monitor::RamStatus ram = ram_monitor.getStatus();

        print_debug_info(cpu, ram);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}