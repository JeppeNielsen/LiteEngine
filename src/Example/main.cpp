#include "ExampleApplication.hpp"

int main() {
    #if defined(__EMSCRIPTEN__)
    static Lite::ExampleApplication app;
    app.Start();
    #else
    Lite::ExampleApplication app;
    app.Start();
    #endif
    return 0;
}
