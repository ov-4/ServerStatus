#include "ui_renderer.h"
#include <emscripten/val.h>
#include <emscripten.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <vector>

using emscripten::val;

namespace web {

std::string FormatSpeed(uint64_t bytes) {
    if (bytes == 0) return "0 B/s";
    const char* sizes[] = {"B/s", "KB/s", "MB/s", "GB/s", "TB/s"};
    int i = 0;
    double dblBytes = static_cast<double>(bytes);
    while (dblBytes >= 1024 && i < 4) {
        dblBytes /= 1024.0;
        i++;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dblBytes << " " << sizes[i];
    return ss.str();
}

std::string FormatTotal(uint64_t bytes) {
    if (bytes == 0) return "0 B";
    const char* sizes[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    int i = 0;
    double dblBytes = static_cast<double>(bytes);
    while (dblBytes >= 1024 && i < 4) {
        dblBytes /= 1024.0;
        i++;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dblBytes << " " << sizes[i];
    return ss.str();
}

std::string GetColorClass(double percentage) {
    if (percentage > 90) return "fill-red";
    if (percentage > 70) return "fill-yellow";
    return "fill-green";
}

std::string GetFlag(const std::string& name) {
    return "🇨🇳"; // TODO: mapping
}


void UIRenderer::Init() {
    val document = val::global("document");
    
    val templateElement = document.call<val>("getElementById", std::string("card-template"));
    
    if (!templateElement.isNull() && !templateElement.isUndefined()) {
        card_template_ = templateElement["innerHTML"].as<std::string>();
        std::cout << "[UIRenderer] Template loaded successfully." << std::endl;
    } else {
        std::cerr << "[UIRenderer] Error: 'card-template' not found in DOM." << std::endl;
        card_template_ = "<div>Error: Template Not Found</div>";
    }
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); 
    }
}

std::string UIRenderer::RenderTemplate(const std::string& templ, 
                                       const std::unordered_map<std::string, std::string>& data) {
    std::string result = templ;
    for (const auto& [key, value] : data) {
        ReplaceAll(result, key, value);
    }
    return result;
}

void UIRenderer::RenderGrid(const nlohmann::json& data) {
    auto document = val::global("document");
    auto summaryGrid = document.call<val>("getElementById", std::string("summary-grid"));
    auto serverGrid = document.call<val>("getElementById", std::string("server-grid"));

    if (data.empty()) {
        serverGrid.set("innerHTML", "<div style='color: #a0aec0; text-align: center; padding: 50px;'>No servers online</div>");
        return;
    }

    int totalServers = data.size();
    int onlineServers = 0; 
    uint64_t totalUpload = 0;
    uint64_t totalDownload = 0;
    uint64_t currentUploadSpeed = 0;
    uint64_t currentDownloadSpeed = 0;

    std::stringstream serverHtml;

    for (const auto& server : data) {
        onlineServers++;
        
        totalUpload += server.value("total_tx", 0ULL); 
        totalDownload += server.value("total_rx", 0ULL);
        currentUploadSpeed += server.value("tx_speed", 0ULL);
        currentDownloadSpeed += server.value("rx_speed", 0ULL);

        serverHtml << CreateCardHTML(server);
    }

    int offlineServers = totalServers - onlineServers;

    std::stringstream summaryHtml;
    summaryHtml << "<div class='summary-card'><div class='summary-title'><span class='status-dot online' style='background:#3b82f6;'></span>Total Servers</div><div class='summary-value'>" << totalServers << "</div></div>";
    summaryHtml << "<div class='summary-card'><div class='summary-title'><span class='status-dot online'></span>Online Servers</div><div class='summary-value'>" << onlineServers << "</div></div>";
    summaryHtml << "<div class='summary-card'><div class='summary-title'><span class='status-dot offline'></span>Offline Servers</div><div class='summary-value'>" << offlineServers << "</div></div>";
    summaryHtml << "<div class='summary-card network-card'><div class='summary-title'>Network</div><div class='network-stats'>"
                << "<div class='net-stat up'><i class='ri-arrow-up-line'></i> <span class='net-total'>" << FormatTotal(totalUpload) << "</span> " << FormatSpeed(currentUploadSpeed) << "</div>"
                << "<div class='net-stat down'><i class='ri-arrow-down-line'></i> <span class='net-total'>" << FormatTotal(totalDownload) << "</span> " << FormatSpeed(currentDownloadSpeed) << "</div>"
                << "</div></div>";

    summaryGrid.set("innerHTML", summaryHtml.str());
    serverGrid.set("innerHTML", serverHtml.str());
}

std::string UIRenderer::CreateCardHTML(const nlohmann::json& s) {
    std::unordered_map<std::string, std::string> map_data;

    std::string id = s.value("id", "Unknown");
    double cpu = s.value("cpu", 0.0);
    double mem_used = s.value("mem_used", 0.0);
    double mem_total = s.value("mem_total", 1.0);
    double disk_used = s.value("disk_used", 0.0);
    double disk_total = s.value("disk_total", 1.0);
    uint64_t tx_speed = s.value("tx_speed", 0);
    uint64_t rx_speed = s.value("rx_speed", 0);

    double memPercent = (mem_used / mem_total) * 100.0;
    double diskPercent = (disk_used / disk_total) * 100.0;
    bool isOnline = true;

    std::stringstream ss_cpu, ss_mem, ss_disk;
    ss_cpu << std::fixed << std::setprecision(1) << cpu;
    ss_mem << std::fixed << std::setprecision(1) << memPercent;
    ss_disk << std::fixed << std::setprecision(1) << diskPercent;

    map_data["{id}"] = id;
    map_data["{flag}"] = GetFlag(id);
    map_data["{status_class}"] = isOnline ? "online" : "offline";
    
    // CPU
    map_data["{cpu_val}"] = ss_cpu.str() + "%";
    map_data["{cpu_width}"] = ss_cpu.str();
    map_data["{cpu_color}"] = GetColorClass(cpu);

    // MEM
    map_data["{mem_val}"] = ss_mem.str() + "%";
    map_data["{mem_width}"] = ss_mem.str();
    map_data["{mem_color}"] = GetColorClass(memPercent);

    // Disk
    map_data["{disk_val}"] = ss_disk.str() + "%";
    map_data["{disk_width}"] = ss_disk.str();
    map_data["{disk_color}"] = GetColorClass(diskPercent);

    // Network
    map_data["{tx_speed}"] = FormatSpeed(tx_speed);
    map_data["{rx_speed}"] = FormatSpeed(rx_speed);

    return RenderTemplate(card_template_, map_data);
}

void UIRenderer::UpdateChartBridge(const nlohmann::json& history_data) {
    std::string json_str = history_data.dump();
    EM_ASM({
        if (window.updateChartsJS) {
            window.updateChartsJS(UTF8ToString($0));
        }
    }, json_str.c_str());
}

} // namespace web