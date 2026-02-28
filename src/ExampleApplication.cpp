#include "ExampleApplication.hpp"

#include "Rendering/Camera.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Renderable.hpp"
#include "Rendering/SokolGfx.hpp"
#include "Rendering/Transform.hpp"

#include "imgui.h"
#include "sokol_glue.h"
#include "sokol_imgui.h"
#include "sokol_log.h"

#include <glm/glm.hpp>

AppConfig ExampleApplication::config() const {
    AppConfig cfg = ApplicationBase::config();
    cfg.title = "Sokol + Premake (macOS)";
    return cfg;
}

void ExampleApplication::on_init() {
    sg_desc desc = {};
    env_ = sglue_environment();
    desc.environment = env_;
    desc.logger.func = slog_func;
    sg_setup(&desc);

    simgui_desc_t ui_desc = {};
    ui_desc.logger.func = slog_func;
    ui_desc.no_default_font = true;
    simgui_setup(&ui_desc);
    ImGui::StyleColorsLight();
    ImGuiIO& io = ImGui::GetIO();
    const float dpi_scale = sapp_dpi_scale();
    const float base_font_size = 12.0f;
    const float font_size = base_font_size * dpi_scale;
    ImFont* font = io.Fonts->AddFontFromFileTTF(imgui_font_path_, font_size);
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

    render_system_.init(env_);

    camera_entity_ = registry_.create();
    Transform camera_transform{};
    camera_transform.position = glm::vec3(0.0f, 0.0f, 2.5f);
    registry_.emplace<Transform>(camera_entity_, camera_transform);
    registry_.emplace<Camera>(camera_entity_);

    cube_entity_ = registry_.create();
    registry_.emplace<Transform>(cube_entity_);
    registry_.emplace<Renderable>(cube_entity_);
    registry_.emplace<Mesh>(cube_entity_, CreateCubeMesh());
}

void ExampleApplication::on_frame() {
    const double dt = sapp_frame_duration();
    if (!pause_rotation_) {
        time_sec_ += dt * static_cast<double>(rotation_speed_);
    }

    simgui_frame_desc_t ui_frame = {};
    ui_frame.width = sapp_width();
    ui_frame.height = sapp_height();
    ui_frame.delta_time = static_cast<float>(dt);
    ui_frame.dpi_scale = sapp_dpi_scale();
    simgui_new_frame(&ui_frame);

    ImGui::Begin("LiteEngine");
    const float fps = ImGui::GetIO().Framerate;
    const float frame_ms = (fps > 0.0f) ? (1000.0f / fps) : 0.0f;
    ImGui::Text("Frame %.3f ms (%.1f FPS)", frame_ms, fps);
    ImGui::Checkbox("Pause rotation", &pause_rotation_);
    ImGui::SliderFloat("Rotation speed", &rotation_speed_, 0.0f, 3.0f, "%.2f");
    ImGui::ColorEdit3("Clear color", clear_color_);
    ImGui::Checkbox("Show ImGui demo", &show_demo_);
    ImGui::End();
    if (show_demo_) {
        ImGui::ShowDemoWindow(&show_demo_);
    }

    if (registry_.valid(cube_entity_)) {
        Transform& cube_transform = registry_.get<Transform>(cube_entity_);
        cube_transform.rotation = glm::vec3(
            static_cast<float>(time_sec_) * 0.7f,
            static_cast<float>(time_sec_) * 1.1f,
            0.0f);
    }

    const float aspect = static_cast<float>(sapp_width()) / static_cast<float>(sapp_height());
    render_system_.begin_frame(sglue_swapchain(), clear_color_);
    render_system_.render(registry_, aspect);
    simgui_render();
    render_system_.end_frame();
}

void ExampleApplication::on_cleanup() {
    simgui_shutdown();
    render_system_.shutdown();
    sg_shutdown();
}

void ExampleApplication::on_event_impl(const sapp_event* ev) {
    simgui_handle_event(ev);
}
