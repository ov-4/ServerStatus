#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace api {

class StatsHandler {
public:
    // handle /api/stats 
    static void Handle(const httplib::Request& req, httplib::Response& res);
};

} // namespace api