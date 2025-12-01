#pragma once

#include <grpcpp/grpcpp.h>
#include "collector.grpc.pb.h"

namespace serverstatus {

class MonitorServiceImpl final : public MonitorService::Service {
public:
    grpc::Status ReportState(grpc::ServerContext* context, const SystemState* request, Ack* reply) override;
};

} // namespace serverstatus