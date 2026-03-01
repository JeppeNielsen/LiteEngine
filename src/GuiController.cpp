#include "GuiController.hpp"

#include "Rendering/SokolGfx.hpp"

#include "imgui.h"
#include "sokol_imgui.h"
#include "sokol_log.h"

void GuiController::init(const char* font_path) {
    if (initialized) {
        return;
    }

    simgui_desc_t ui_desc = {};
    ui_desc.logger.func = slog_func;
    ui_desc.no_default_font = true;
    simgui_setup(&ui_desc);

    ImGui::StyleColorsLight();
    ImGuiIO& io = ImGui::GetIO();
    const float dpi_scale = sapp_dpi_scale();
    const float base_font_size = 12.0f;
    const float font_size = base_font_size * dpi_scale;
    ImFont* font = nullptr;
    if (font_path && font_path[0] != '\0') {
        font = io.Fonts->AddFontFromFileTTF(font_path, font_size);
    }
    if (!font) {
        ImFontConfig font_cfg = {};
        font_cfg.SizePixels = font_size;
        font = io.Fonts->AddFontDefault(&font_cfg);
    }
    if (font) {
        io.FontDefault = font;
    }
    if (dpi_scale > 0.0f) {
        io.FontGlobalScale = 1.0f / dpi_scale;
    }

    initialized = true;
}

void GuiController::begin_frame(double dt) {
    if (!initialized) {
        return;
    }

    simgui_frame_desc_t ui_frame = {};
    ui_frame.width = sapp_width();
    ui_frame.height = sapp_height();
    ui_frame.delta_time = static_cast<float>(dt);
    ui_frame.dpi_scale = sapp_dpi_scale();
    simgui_new_frame(&ui_frame);
}

void GuiController::render() {
    if (!initialized) {
        return;
    }

    simgui_render();
}

void GuiController::shutdown() {
    if (!initialized) {
        return;
    }

    simgui_shutdown();
    initialized = false;
}

void GuiController::handle_event(const sapp_event* ev) {
    if (!initialized) {
        return;
    }

    simgui_handle_event(ev);
}
