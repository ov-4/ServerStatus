#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace web {

class UIRenderer {
public:
    // init html templete
    void Init();

    void RenderGrid(const nlohmann::json& data);
    void UpdateChartBridge(const nlohmann::json& history_data);

private:
    std::string CreateCardHTML(const nlohmann::json& server);
    
    // replace
    std::string RenderTemplate(const std::string& templ, 
                               const std::unordered_map<std::string, std::string>& data);

    std::string FormatBytes(uint64_t bytes);

    // get template str
    std::string card_template_;
};

} // namespace web