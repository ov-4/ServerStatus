#pragma once

#include <cstdint>
#include <string>

namespace monitor {

struct DiskStatus {
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint64_t free_bytes;
};

class DiskMonitor {
public:
    // default "/"
    DiskStatus getStatus(const std::string& path = "/");
};

} // namespace monitor