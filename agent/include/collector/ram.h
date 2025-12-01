#pragma once

#include "collector.pb.h"

namespace collector {

class RamMonitor {
public:
    void Collect(serverstatus::SystemState* state);
};

} // namespace collector