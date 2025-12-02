#pragma once
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include "collector.pb.h"

namespace collector {

struct PingResult {
    std::string id;
    double latency;
    double loss;
};

class SpeedtestExecutor {
public:
    static double PingIcmp(const std::string& host);
    static double PingTcp(const std::string& target);
    static double PingHttp(const std::string& url);

    // queue management for async results
    static void AddResult(const PingResult& res);
    static std::vector<PingResult> PopAllResults();

private:
    static std::mutex queue_mutex_;
    static std::vector<PingResult> result_queue_;
};

}