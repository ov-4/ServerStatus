#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace api {

class HistoryHandler {
public:
    // handle /api/history
    static void Handle(const httplib::Request& req, httplib::Response& res);
};

} // namespace api