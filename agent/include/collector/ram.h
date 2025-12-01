#pragma once

#include "collector.pb.h"

namespace monitor {

class RamMonitor {
public:
    void Collect(serverstatus::SystemState* state);
};

} // namespace monitor