#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <deque>
#include <nlohmann/json.hpp>
#include "collector.pb.h"
#include "server_config.h"

namespace serverstatus {

class Storage {
public:
    static Storage& Instance() {
        static Storage instance;
        return instance;
    }

    void Update(const std::string& id, const SystemState& state) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto& history = clients_[id];
        
        history.push_back(state);
        
        int max_size = ServerConfig::Instance().Get().history_size;
        if (history.size() > static_cast<size_t>(max_size)) {
            history.pop_front();
        }
    }

    std::vector<nlohmann::json> GetAllAsJson() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<nlohmann::json> result;

        for (const auto& [id, history] : clients_) {
            if (history.empty()) continue;
            
            const auto& state = history.back();
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

    nlohmann::json GetHistoryAsJson(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json result = nlohmann::json::array();
    
    if (clients_.find(id) == clients_.end()) return result;

    const auto& history = clients_[id];
    for (const auto& state : history) {
        nlohmann::json j;
        j["cpu"] = state.cpu_usage();
        j["mem_used"] = state.memory_used();
        j["mem_total"] = state.memory_total();
        j["rx_speed"] = state.network_rx_speed();
        j["tx_speed"] = state.network_tx_speed();
        
        j["speedtest"] = nlohmann::json::array();
        for (const auto& st : state.speedtest_results()) {
            nlohmann::json s_json;
            s_json["id"] = st.id();
            s_json["latency"] = st.latency_ms();
            s_json["loss"] = st.packet_loss();
            j["speedtest"].push_back(s_json);
        }
        
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
    std::map<std::string, std::deque<SystemState>> clients_;
};

} // namespace serverstatus