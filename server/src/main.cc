// server/src/main.cc
#include <iostream>
#include "server_config.h"
#include "handlers/update_handler.h"

// 必须定义这个宏才能使用 httplib (如果库是 FetchContent 下来的通常自带，但为了保险)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

int main() {
    // 实例化 Server
    httplib::Server svr;

    // 绑定路由：POST /post/update -> handle_update
    svr.Post("/post/update", handler::handle_update);

    // 启动监听
    std::cout << "ServerStatus Server is running..." << std::endl;
    std::cout << "Listening on " << SERVER_HOST << ":" << SERVER_PORT << std::endl;

    // listen 会阻塞主线程
    if (!svr.listen(SERVER_HOST, SERVER_PORT)) {
        std::cerr << "[Error] Failed to bind to port " << SERVER_PORT << std::endl;
        return 1;
    }

    return 0;
}