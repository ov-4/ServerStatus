#include "monitors/disk.h"
#include <sys/statvfs.h>

namespace monitor {

DiskStatus DiskMonitor::getStatus(const std::string& path) {
    DiskStatus status{0, 0, 0};
    struct statvfs buf;

    if (statvfs(path.c_str(), &buf) == 0) {
        // f_frsize is fragment size，usually better than f_bsize
        uint64_t total = buf.f_blocks * buf.f_frsize;
        uint64_t free = buf.f_bfree * buf.f_frsize; // f_bfree is total available;f_bavail is non-root-user available
        
        status.total_bytes = total;
        status.free_bytes = free;
        status.used_bytes = total - free;
    }

    return status;
}

} // namespace monitor