#include "client.h"
#include "agent_config.h"
#include "collector/speedtest.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace collector {

    void RunSpeedtestTasks(const serverstatus::Ack& ack) {
    std::vector<serverstatus::SpeedtestTask> tasks;
    for (const auto& t : ack.speedtest_tasks()) {
        tasks.push_back(t);
    }
    
    if (tasks.empty()) return;

    std::thread([tasks]() {
        for (const auto& task : tasks) {
            double latency = 0.0;
            if (task.type() == "icmp") {
                latency = SpeedtestExecutor::PingIcmp(task.target());
            } else if (task.type() == "tcp") {
                latency = SpeedtestExecutor::PingTcp(task.target());
            } else if (task.type() == "get") {
                latency = SpeedtestExecutor::PingHttp(task.target());
            }

            // packet loss logic
            double loss = (latency > 0) ? 0.0 : 100.0;
            SpeedtestExecutor::AddResult({task.id(), latency, loss});
        }
    }).detach();
}

AgentClient::AgentClient(const std::string& server_address) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(
        server_address, 
        grpc::InsecureChannelCredentials()
    );
    
    stub_ = serverstatus::MonitorService::NewStub(channel);
}

void AgentClient::Run() {
    std::cout << "Agent started. Reporting loop running..." << std::endl;

    while (true) {
        auto config = AgentConfigLoader::Instance().Get();

        serverstatus::SystemState state;

        state.set_uuid(config->uuid);
        state.set_token(config->token);

        cpu_mon_.Collect(&state);
        ram_mon_.Collect(&state);
        disk_mon_.Collect(&state, "/");
        net_mon_.Collect(&state);

        // get pending speedtest results
        auto results = SpeedtestExecutor::PopAllResults();
        for (const auto& res : results) {
            auto* r = state.add_speedtest_results();
            r->set_id(res.id);
            r->set_latency_ms(res.latency);
            r->set_packet_loss(res.loss);
        }

        grpc::ClientContext context;
        serverstatus::Ack ack;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));
        
        grpc::Status status = stub_->ReportState(&context, state, &ack);

        if (status.ok()) {
            if (!ack.success()) {
                std::cerr << "[Server Error] " << ack.message() << std::endl;
            } else {
                RunSpeedtestTasks(ack);
            }
        } else {
            std::cerr << "[RPC Error] " << status.error_code() 
                      << ": " << status.error_message() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace collector