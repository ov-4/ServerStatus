#include "network.h"
#include <iostream>

namespace web {

void NetworkClient::Get(const std::string& url, SuccessCallback on_success, ErrorCallback on_error) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

    // use user_data to pass pointer
    // notice memory management
    auto* cbs = new std::pair<SuccessCallback, ErrorCallback>(on_success, on_error);
    attr.userData = static_cast<void*>(cbs);
    
    attr.onsuccess = [](emscripten_fetch_t* fetch) {
        auto* cbs = static_cast<std::pair<SuccessCallback, ErrorCallback>*>(fetch->userData);
        if (cbs && cbs->first) {
            std::string data(fetch->data, fetch->numBytes);
            cbs->first(data);
        }
        delete cbs; // clean
        emscripten_fetch_close(fetch);
    };
    
    attr.onerror = [](emscripten_fetch_t* fetch) {
        auto* cbs = static_cast<std::pair<SuccessCallback, ErrorCallback>*>(fetch->userData);
        std::cerr << "Fetch failed for URL: " << fetch->url << std::endl;
        if (cbs && cbs->second) {
            cbs->second(fetch->status);
        }
        delete cbs;
        emscripten_fetch_close(fetch);
    };

    emscripten_fetch(&attr, url.c_str());
}

}