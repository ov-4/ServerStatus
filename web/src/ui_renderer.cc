#include "ui_renderer.h"
#include <emscripten/val.h>
#include <emscripten.h> // for EM_ASM
#include <sstream>
#include <iomanip>
#include <cmath>

using emscripten::val;

namespace web {

void UIRenderer::RenderGrid(const nlohmann::json& data) {
    auto document = val::global("document");
    auto grid = document.call<val>("getElementById", std::string("server-grid"));

    if (data.empty()) {
        grid.set("innerHTML", "<div style='padding:40px;text-align:center;'>No servers online</div>");
        return;
    }

    std::stringstream html;
    for (const auto& server : data) {
        html << CreateCardHTML(server);
    }
    
    grid.set("innerHTML", html.str());
}

std::string UIRenderer::FormatBytes(uint64_t bytes) {
    if (bytes == 0) return "0 B";
    const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = static_cast<int>(floor(log(bytes) / log(1024)));
    double val = bytes / pow(1024, i);
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << val << " " << sizes[i];
    return ss.str();
}

std::string UIRenderer::CreateCardHTML(const nlohmann::json& s) {
    
    double cpu = s.value("cpu", 0.0);
    
    std::stringstream ss;
    ss << "<div class='card' onclick=\"window.openDetail('" << s["id"].get<std::string>() << "')\">";
    ss << "<div class='server-name'>" << s["id"].get<std::string>() << "</div>";
    ss << "<span>CPU: " << std::fixed << std::setprecision(1) << cpu << "%</span>";
    ss << "</div>";
    return ss.str();
}

void UIRenderer::UpdateChartBridge(const nlohmann::json& history_data) {

    std::string json_str = history_data.dump();
    

    EM_ASM({
        if (window.updateChartsJS) {
            window.updateChartsJS(UTF8ToString($0));
        }
    }, json_str.c_str());
}

}