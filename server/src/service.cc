#include "service.h"
#include "storage.h"
#include "server_config.h"
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
    std::string client_id = context->peer();

    std::string uuid = request->uuid();
    std::string token = request->token();

    if (!ServerConfig::Instance().Verify(uuid, token)) {
        std::cerr << "[Auth] Unauthorized access from " << client_id
                  << " (UUID: " << uuid << ")" << std::endl;
        return grpc::Status(grpc::PERMISSION_DENIED, "Authentication failed");
    }

    std::string storage_id = uuid.empty() ? client_id : uuid;

    Storage::Instance().Update(storage_id, *request);

    std::cout << "[RPC] Update from: " << storage_id << std::endl;

    reply->set_success(true);
    reply->set_message("OK");

    return grpc::Status::OK;
}

} // namespace serverstatus