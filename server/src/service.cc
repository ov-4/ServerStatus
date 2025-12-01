#include "service.h"
#include "storage.h"
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
    // 获取客户端标识 (IP:Port)
    std::string client_id = context->peer();

    // [新增] 将数据存入内存
    Storage::Instance().Update(client_id, *request);

    // 依然保留日志打印，方便调试
    std::cout << "[RPC] Update from: " << client_id << std::endl;

    reply->set_success(true);
    reply->set_message("OK");

    return grpc::Status::OK;
}

} // namespace serverstatus