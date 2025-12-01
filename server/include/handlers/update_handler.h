#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace handler {

void handle_update(const httplib::Request& req, httplib::Response& res);

} // namespace handler