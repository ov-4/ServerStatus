#include "handlers/update_handler.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip> // setprecision
#include <string>

using json = nlohmann::json;

namespace handler {

std::string simple_format_bytes(uint64_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    return std::to_string(bytes / 1024 / 1024) + " MB";
}

void handle_update(const httplib::Request& req, httplib::Response& res) {
    try {
        // json body
        auto data = json::parse(req.body);

        // keys
        double cpu = data.value("cpu", 0.0);
        
        // format
        std::cout << "[Received] IP: " << std::setw(15) << std::left << req.remote_addr 
                  << " | CPU: " << std::fixed << std::setprecision(1) << cpu << "%";

        if (data.contains("ram_usage")) {
             std::cout << " | RAM: " << std::fixed << std::setprecision(1) << data["ram_usage"] << "%";
        }
        
        if (data.contains("rx_speed") && data.contains("tx_speed")) {
            uint64_t rx = data["rx_speed"];
            uint64_t tx = data["tx_speed"];
            std::cout << " | Net: ↓" << simple_format_bytes(rx) << "/s ↑" << simple_format_bytes(tx) << "/s";
        }

        std::cout << std::endl;

        // respond
        res.set_content("Success", "text/plain");

    } catch (const json::parse_error& e) {
        // JSON parse error
        std::cerr << "[Error] JSON Parse failed from " << req.remote_addr << ": " << e.what() << std::endl;
        res.status = 400;
        res.set_content("Invalid JSON", "text/plain");
    } catch (const std::exception& e) {
        std::cerr << "[Error] Unknown error: " << e.what() << std::endl;
        res.status = 500;
    }
}

} // namespace handler