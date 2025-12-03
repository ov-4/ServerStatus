#include "app.h"

int main() {
    auto* app = new web::App();
    app->Init();
    app->RunLoop();
    
    return 0;
}