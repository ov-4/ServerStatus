#pragma once

#include "network.h"
#include "ui_renderer.h"

namespace web {

class App {
public:
    void Init();
    void RunLoop(); // timer

    // interface for JS
    void OnDetailViewOpened(std::string id);
    void OnBackToDashboard();

private:
    void FetchStats();
    void FetchHistory(const std::string& id);

    UIRenderer renderer_;
    std::string current_detail_id_;
    bool is_detail_view_ = false;
};

}