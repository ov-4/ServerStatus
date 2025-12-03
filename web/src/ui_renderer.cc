#include "ui_renderer.h"
#include <emscripten/val.h>
#include <emscripten.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <numeric>

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
    return "🇨🇳"; //TODO: national flag mapping
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
        bool isOnline = true; 
        if (isOnline) onlineServers++;

        totalUpload += server.value("total_tx", 0ULL); 
        totalDownload += server.value("total_rx", 0ULL);
        
        currentUploadSpeed += server.value("tx_speed", 0ULL);
        currentDownloadSpeed += server.value("rx_speed", 0ULL);

        serverHtml << CreateCardHTML(server);
    }

    int offlineServers = totalServers - onlineServers;

    // 2. 渲染概览卡片 (Summary Cards)
    std::stringstream summaryHtml;
    // Total Servers
    summaryHtml << "<div class='summary-card'>"
                << "<div class='summary-title'><span class='status-dot online' style='background:#3b82f6;'></span>Total Servers</div>"
                << "<div class='summary-value'>" << totalServers << "</div>"
                << "</div>";
    // Online Servers
    summaryHtml << "<div class='summary-card'>"
                << "<div class='summary-title'><span class='status-dot online'></span>Online Servers</div>"
                << "<div class='summary-value'>" << onlineServers << "</div>"
                << "</div>";
    // Offline Servers
    summaryHtml << "<div class='summary-card'>"
                << "<div class='summary-title'><span class='status-dot offline'></span>Offline Servers</div>"
                << "<div class='summary-value'>" << offlineServers << "</div>"
                << "</div>";
    // Network
    summaryHtml << "<div class='summary-card network-card'>"
                << "<div class='summary-title'>Network</div>"
                << "<div class='network-stats'>"
                << "<div class='net-stat up'><i class='ri-arrow-up-line'></i> <span class='net-total'>" << FormatTotal(totalUpload) << "</span> " << FormatSpeed(currentUploadSpeed) << "</div>"
                << "<div class='net-stat down'><i class='ri-arrow-down-line'></i> <span class='net-total'>" << FormatTotal(totalDownload) << "</span> " << FormatSpeed(currentDownloadSpeed) << "</div>"
                << "</div>"
                << "</div>";

    summaryGrid.set("innerHTML", summaryHtml.str());
    
    serverGrid.set("innerHTML", serverHtml.str());
}

std::string UIRenderer::CreateCardHTML(const nlohmann::json& s) {
    std::string id = s.value("id", "Unknown");
    bool isOnline = true;
    std::string statusClass = isOnline ? "online" : "offline";
    std::string cardClass = isOnline ? "" : "offline";

    double cpu = s.value("cpu", 0.0);
    double mem_used = s.value("mem_used", 0.0);
    double mem_total = s.value("mem_total", 1.0);
    double disk_used = s.value("disk_used", 0.0);
    double disk_total = s.value("disk_total", 1.0);
    uint64_t tx_speed = s.value("tx_speed", 0);
    uint64_t rx_speed = s.value("rx_speed", 0);

    double memPercent = (mem_used / mem_total) * 100.0;
    double diskPercent = (disk_used / disk_total) * 100.0;

    std::stringstream ss;
    ss << "<div class='server-card " << cardClass << "' onclick=\"window.openDetail('" << id << "')\">";

    // --- Header (Left) ---
    ss << "<div class='server-header'>";
    ss << "<span class='flag'>" << GetFlag(id) << "</span>";
    ss << "<div class='server-info'>";
    ss << "<div class='server-name'>" << id << "</div>";
    ss << "<span class='status-dot " << statusClass << "'></span>";
    ss << "</div>";
    ss << "</div>"; // end server-header

    // --- Stats (Right - 5 Columns) ---
    ss << "<div class='server-stats'>";

    // 1. CPU
    ss << "<div class='stat-item'>";
    ss << "<div class='stat-header'><span>CPU</span><span class='stat-value'>" << std::fixed << std::setprecision(2) << cpu << "%</span></div>";
    ss << "<div class='progress-bar'><div class='progress-fill " << GetColorClass(cpu) << "' style='width:" << cpu << "%'></div></div>";
    ss << "</div>";

    // 2. MEM
    ss << "<div class='stat-item'>";
    ss << "<div class='stat-header'><span>MEM</span><span class='stat-value'>" << std::fixed << std::setprecision(2) << memPercent << "%</span></div>";
    ss << "<div class='progress-bar'><div class='progress-fill " << GetColorClass(memPercent) << "' style='width:" << memPercent << "%'></div></div>";
    ss << "</div>";

    // 3. STG (Storage)
    ss << "<div class='stat-item'>";
    ss << "<div class='stat-header'><span>STG</span><span class='stat-value'>" << std::fixed << std::setprecision(2) << diskPercent << "%</span></div>";
    ss << "<div class='progress-bar'><div class='progress-fill " << GetColorClass(diskPercent) << "' style='width:" << diskPercent << "%'></div></div>";
    ss << "</div>";

    // 4. Upload
    ss << "<div class='stat-item'>";
    ss << "<div class='stat-header'><span>Upload</span><span class='stat-value'>" << FormatSpeed(tx_speed) << "</span></div>";
    ss << "<div class='progress-bar' style='background:none;'></div>"; 
    ss << "</div>";

    // 5. Download
    ss << "<div class='stat-item'>";
    ss << "<div class='stat-header'><span>Download</span><span class='stat-value'>" << FormatSpeed(rx_speed) << "</span></div>";
    ss << "<div class='progress-bar' style='background:none;'></div>";
    ss << "</div>";

    ss << "</div>"; // end server-stats
    ss << "</div>"; // end server-card

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

} // namespace web