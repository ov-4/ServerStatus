#include "api/stats_handler.h"
#include "storage.h"
#include <nlohmann/json.hpp>

namespace api {

void StatsHandler::Handle(const httplib::Request& req, httplib::Response& res) {
    auto stats_list = serverstatus::Storage::Instance().GetAllAsJson();
    
    nlohmann::json json_response = stats_list;
    
    // allow CORS
    res.set_header("Access-Control-Allow-Origin", "*"); 
    res.set_content(json_response.dump(), "application/json");
}

} // namespace api