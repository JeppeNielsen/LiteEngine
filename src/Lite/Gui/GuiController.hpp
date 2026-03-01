#pragma once

#include "Lite/Platform/Sokol/SokolApp.hpp"

struct sg_swapchain;

namespace Lite {

class GuiController {
public:
    void init(const char* font_path);
    void begin_frame(double dt);
    void render(sg_swapchain swapchain);
    void shutdown();
    void handle_event(const sapp_event* ev);

    bool is_initialized() const { return initialized; }

private:
    bool initialized = false;
};

} // namespace Lite
