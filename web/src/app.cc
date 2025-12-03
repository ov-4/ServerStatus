#include "app.h"
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <iostream>

namespace web {

static App* g_app = nullptr;

void App::Init() {
    g_app = this;
    std::cout << "[Wasm] App Initialized." << std::endl;
    FetchStats();
}

void App::RunLoop() {
    emscripten_set_interval([](void* user_data) {
        App* app = static_cast<App*>(user_data);
        if (app) {
            if (app->is_detail_view_ && !app->current_detail_id_.empty()) {
                app->FetchHistory(app->current_detail_id_);
            } else {
                app->FetchStats();
            }
        }
    }, 1000.0, this);
}

void App::FetchStats() {
    NetworkClient::Get("/api/stats", [this](const std::string& data) {
        try {
            auto json = nlohmann::json::parse(data);
            if (!is_detail_view_) {
                renderer_.RenderGrid(json);
            }
        } catch (...) {
            std::cerr << "JSON parse error in FetchStats" << std::endl;
        }
    });
}

void App::FetchHistory(const std::string& id) {
    std::string url = "/api/history?id=" + id;
    NetworkClient::Get(url, [this](const std::string& data) {
        try {
            auto json = nlohmann::json::parse(data);
            renderer_.UpdateChartBridge(json);
        } catch (...) {}
    });
}

void App::OnDetailViewOpened(std::string id) {
    std::cout << "Open detail: " << id << std::endl;
    current_detail_id_ = id;
    is_detail_view_ = true;
    
    emscripten::val doc = emscripten::val::global("document");
    doc.call<emscripten::val>("getElementById", std::string("dashboard-view")).set("style", "display:none");
    doc.call<emscripten::val>("getElementById", std::string("detail-view")).set("style", "display:block");
    doc.call<emscripten::val>("getElementById", std::string("detail-title")).set("innerText", id);
    
    FetchHistory(id);
}

void App::OnBackToDashboard() {
    current_detail_id_ = "";
    is_detail_view_ = false;
    
    emscripten::val doc = emscripten::val::global("document");
    doc.call<emscripten::val>("getElementById", std::string("dashboard-view")).set("style", "display:block");
    doc.call<emscripten::val>("getElementById", std::string("detail-view")).set("style", "display:none");
}

} // namespace web

void CPP_OpenDetail(std::string id) {
    if (web::g_app) web::g_app->OnDetailViewOpened(id);
}

void CPP_BackToDashboard() {
    if (web::g_app) web::g_app->OnBackToDashboard();
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("cppOpenDetail", &CPP_OpenDetail);
    emscripten::function("cppBackToDashboard", &CPP_BackToDashboard);
}