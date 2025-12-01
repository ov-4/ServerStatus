#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "collector.pb.h"

namespace serverstatus {

class Storage {
public:
    static Storage& Instance() {
        static Storage instance;
        return instance;
    }

    void Update(const std::string& id, const SystemState& state) {
        std::lock_guard<std::mutex> lock(mutex_);
        clients_[id] = state;
    }

    std::vector<nlohmann::json> GetAllAsJson() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<nlohmann::json> result;

        for (const auto& [id, state] : clients_) {
            nlohmann::json j;
            j["id"] = id;
            j["cpu"] = state.cpu_usage();
            j["mem_used"] = state.memory_used();
            j["mem_total"] = state.memory_total();
            j["disk_used"] = state.disk_used();
            j["disk_total"] = state.disk_total();
            j["rx_speed"] = state.network_rx_speed();
            j["tx_speed"] = state.network_tx_speed();
            j["uptime"] = state.uptime();
            result.push_back(j);
        }
        return result;
    }

private:
    Storage() = default;
    ~Storage() = default;
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    std::mutex mutex_;
    std::map<std::string, SystemState> clients_;
};

} // namespace serverstatus