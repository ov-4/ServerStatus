#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace web {

class UIRenderer {
public:
    void RenderGrid(const nlohmann::json& data);
    void UpdateChartBridge(const nlohmann::json& history_data); // JS update chart
                                                                // TODO: use WebAssembly

private:
    std::string CreateCardHTML(const nlohmann::json& server);
    std::string FormatBytes(uint64_t bytes);
};

}