#include "client.h"
#include "agent_config.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace collector {

AgentClient::AgentClient(const std::string& server_address) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(
        server_address, 
        grpc::InsecureChannelCredentials()
    );
    
    stub_ = serverstatus::MonitorService::NewStub(channel);
}

void AgentClient::Run() {
    std::cout << "Agent started. Reporting loop running..." << std::endl;

    const auto& config = AgentConfig::Instance().Get();

    while (true) {
        serverstatus::SystemState state;

        state.set_uuid(config.uuid);
        state.set_token(config.token);

        cpu_mon_.Collect(&state);
        ram_mon_.Collect(&state);
        disk_mon_.Collect(&state, "/");
        net_mon_.Collect(&state);

        grpc::ClientContext context;
        serverstatus::Ack ack;

        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

        grpc::Status status = stub_->ReportState(&context, state, &ack);

        if (status.ok()) {
            if (!ack.success()) {
                std::cerr << "[Server Error] " << ack.message() << std::endl;
            }
        } else {
            std::cerr << "[RPC Error] " << status.error_code() 
                      << ": " << status.error_message() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace collector