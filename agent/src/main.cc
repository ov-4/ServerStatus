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

#define SERVER_ADDRESS "127.0.0.1:8080"

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

int main(int argc, char** argv) {
    std::string server_address = SERVER_ADDRESS;
    if (argc > 1) {
        server_address = argv[1];
    }

    std::cout << "Starting ServerStatus Agent (gRPC Mode)..." << std::endl;
    std::cout << "Target Server: " << server_address << std::endl;

    monitor::AgentClient client(server_address);

    client.Run();

    return 0;
}