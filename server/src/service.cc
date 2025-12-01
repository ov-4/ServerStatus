#include "service.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

namespace serverstatus {

std::string FormatBytes(uint64_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblBytes = static_cast<double>(bytes);
    while (dblBytes >= 1024.0 && i < 4) {
        dblBytes /= 1024.0;
        i++;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dblBytes << " " << suffixes[i];
    return ss.str();
}

grpc::Status MonitorServiceImpl::ReportState(grpc::ServerContext* context, const SystemState* request, Ack* reply) {
    std::cout << "[RPC] Client: " << context->peer() << " | "
              << "CPU: " << std::fixed << std::setprecision(1) << request->cpu_usage() << "% | "
              << "RAM: " << FormatBytes(request->memory_used()) << " / " << FormatBytes(request->memory_total()) << " | "
              << "Net: ↓" << FormatBytes(request->network_rx_speed()) << "/s ↑" << FormatBytes(request->network_tx_speed()) << "/s"
              << std::endl;

    reply->set_success(true);
    reply->set_message("OK");

    return grpc::Status::OK;
}

} // namespace serverstatus