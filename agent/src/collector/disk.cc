#include "collector/disk.h"
#include <sys/statvfs.h>

namespace monitor {

void DiskMonitor::Collect(serverstatus::SystemState* state, const std::string& path) {
    struct statvfs buf;

    if (statvfs(path.c_str(), &buf) == 0) {
        uint64_t total = buf.f_blocks * buf.f_frsize;
        uint64_t free = buf.f_bfree * buf.f_frsize; 
        uint64_t used = total - free;

        state->set_disk_total(total);
        state->set_disk_used(used);
    } else {
        state->set_disk_total(0);
        state->set_disk_used(0);
    }
}

} // namespace monitor