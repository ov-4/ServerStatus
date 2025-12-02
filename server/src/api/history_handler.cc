#include "api/history_handler.h"
#include "storage.h"
#include <nlohmann/json.hpp>

namespace api {

void HistoryHandler::Handle(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");

    if (req.has_param("id")) {
        std::string id = req.get_param_value("id");
        
        auto history = serverstatus::Storage::Instance().GetHistoryAsJson(id);
        
        res.set_content(history.dump(), "application/json");
    } else {
        res.status = 400;
        res.set_content("Error: Missing 'id' parameter. usage: /api/history?id=uuid", "text/plain");
    }
}

} // namespace api