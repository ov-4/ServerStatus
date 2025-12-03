#include "collector/speedtest.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>
#include <sstream>
#include <chrono>
#include <thread>
#include <regex>

// for tcp ping
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

// httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

// TODO: add timeout to config
#define TIMEOUT_SECONDS 2

namespace collector {

std::mutex SpeedtestExecutor::queue_mutex_;
std::vector<PingResult> SpeedtestExecutor::result_queue_;

void SpeedtestExecutor::AddResult(const PingResult& res) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    result_queue_.push_back(res);
}

std::vector<PingResult> SpeedtestExecutor::PopAllResults() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    std::vector<PingResult> temp = result_queue_;
    result_queue_.clear();
    return temp;
}


double SpeedtestExecutor::PingIcmp(const std::string& host) {
    // use system `ping` to avoid need of raw socket permissions (aka root)
    // may support custom raw method later

    const std::string allowed_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-";
    for (char &c : host) {
        if (allowed_chars.find(c) == std::string::npos) {
            return 0.0;
        }
    }


    std::string cmd = "ping -c 1 -W 1 " + host + " 2>&1";
    std::array<char, 128> buffer;
    std::string result;

    struct PipeCloser {
        void operator()(FILE* fp) const {
            if (fp) pclose(fp);
        }
    };
    
    std::unique_ptr<FILE, PipeCloser> pipe(popen(cmd.c_str(), "r"));
    if (!pipe) return 0.0;
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    // parse time=xx.x ms
    std::regex time_regex("time=([0-9.]+)");
    std::smatch match;
    if (std::regex_search(result, match, time_regex) && match.size() > 1) {
        return std::stod(match.str(1));
    }
    
    return 0.0; // timeout or error
}

double SpeedtestExecutor::PingTcp(const std::string& target) {
    // ip port
    size_t colon = target.find(':');
    if (colon == std::string::npos) return 0.0;
    
    std::string ip = target.substr(0, colon);
    int port = std::stoi(target.substr(colon + 1));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return 0.0;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    // timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECONDS; // second
    timeout.tv_usec = 0; // microsecond
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    auto start = std::chrono::high_resolution_clock::now();
    int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    auto end = std::chrono::high_resolution_clock::now();

    close(sock);

    if (res == 0) {
        std::chrono::duration<double, std::milli> elapsed = end - start;
        return elapsed.count();
    }
    return 0.0;
}

double SpeedtestExecutor::PingHttp(const std::string& url_str) {
    // http://domain/path
    // simplified parser
    // TODO: may support https later
    // so that tls latency can also be checked
    size_t proto_end = url_str.find("://");
    if (proto_end == std::string::npos) return 0.0;
    
    std::string proto = url_str.substr(0, proto_end);
    std::string rest = url_str.substr(proto_end + 3);
    
    size_t path_start = rest.find('/');
    std::string domain = (path_start == std::string::npos) ? rest : rest.substr(0, path_start);
    std::string path = (path_start == std::string::npos) ? "/" : rest.substr(path_start);

    auto start = std::chrono::high_resolution_clock::now();
    
    httplib::Client cli(proto + "://" + domain);
    cli.set_connection_timeout(2, 0);
    auto res = cli.Get(path.c_str());
    
    auto end = std::chrono::high_resolution_clock::now();

    if (res) {
        std::chrono::duration<double, std::milli> elapsed = end - start;
        return elapsed.count();
    }
    return 0.0;
}

}