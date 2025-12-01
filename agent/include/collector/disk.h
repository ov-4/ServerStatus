#pragma once

#include "collector.pb.h"
#include <string>

namespace collector {

class DiskMonitor {
public:
    void Collect(serverstatus::SystemState* state, const std::string& path = "/");
};

} // namespace collector