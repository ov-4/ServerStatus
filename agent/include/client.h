#pragma once

#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "collector.grpc.pb.h"

#include "collector/cpu.h"
#include "collector/ram.h"
#include "collector/disk.h"
#include "collector/network.h"

namespace monitor {

class AgentClient {
public:
    explicit AgentClient(const std::string& server_address);

    void Run();

private:
    std::unique_ptr<serverstatus::MonitorService::Stub> stub_;

    CpuMonitor cpu_mon_;
    RamMonitor ram_mon_;
    DiskMonitor disk_mon_;
    NetworkMonitor net_mon_;
};

} // namespace monitor