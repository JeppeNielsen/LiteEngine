#pragma once

#include "Lite/Platform/Sokol/SokolApp.hpp"

namespace Lite {

class GuiController {
public:
    void init(const char* font_path);
    void begin_frame(double dt);
    void render();
    void shutdown();
    void handle_event(const sapp_event* ev);

    bool is_initialized() const { return initialized; }

private:
    bool initialized = false;
};

} // namespace Lite
