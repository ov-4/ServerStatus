#pragma once

#include <string>
#include <functional>
#include <emscripten/fetch.h>

namespace web {

class NetworkClient {
public:
    using SuccessCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(int)>;

    // GET request
    static void Get(const std::string& url, SuccessCallback on_success, ErrorCallback on_error = nullptr);

private:
    static void OnSuccess(emscripten_fetch_t* fetch);
    static void OnError(emscripten_fetch_t* fetch);
};

}