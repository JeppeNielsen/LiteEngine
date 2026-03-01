#include "ExampleApplication.hpp"

#include "Lite/Rendering/Camera.hpp"
#include "Lite/Rendering/Mesh.hpp"
#include "Lite/Rendering/Renderable.hpp"
#include "Lite/Rendering/SokolGfx.hpp"
#include "Lite/Rendering/Transform.hpp"

#include "Lite/Platform/Sokol/SokolApp.hpp"
#include "imgui.h"
#include "sokol_glue.h"
#include "sokol_imgui.h"
#include "sokol_log.h"

#include <glm/glm.hpp>

namespace Lite {

AppConfig ExampleApplication::config() const {
    AppConfig cfg = ApplicationBase::config();
    cfg.title = "Sokol + Premake (macOS)";
    return cfg;
}

void ExampleApplication::on_init() {
    render_system.init(environment());

    camera_entity = registry.create();
    Transform camera_transform{};
    camera_transform.position = glm::vec3(0.0f, 0.0f, 2.5f);
    registry.emplace<Transform>(camera_entity, camera_transform);
    registry.emplace<Camera>(camera_entity);

    cube_entity = registry.create();
    registry.emplace<Transform>(cube_entity);
    registry.emplace<Renderable>(cube_entity);
    registry.emplace<Mesh>(cube_entity, CreateCubeMesh());
}

void ExampleApplication::on_frame(double dt) {
    if (!pause_rotation) {
        time_sec += dt * static_cast<double>(rotation_speed);
    }

    if (registry.valid(cube_entity)) {
        Transform& cube_transform = registry.get<Transform>(cube_entity);
        cube_transform.rotation = glm::vec3(
            static_cast<float>(time_sec) * 0.7f,
            static_cast<float>(time_sec) * 1.1f,
            0.0f);
    }

    const sg_view target_view = render_system.render_target_view();
    if (viewport_width > 0 && viewport_height > 0 && target_view.id) {
        const float aspect = static_cast<float>(viewport_width) / static_cast<float>(viewport_height);
        render_system.begin_offscreen_frame(clear_color);
        render_system.render(registry, aspect);
        render_system.end_frame();
    }
}

void ExampleApplication::on_gui() {
    ImGui::Begin("LiteEngine");
    const float fps = ImGui::GetIO().Framerate;
    const float frame_ms = (fps > 0.0f) ? (1000.0f / fps) : 0.0f;
    ImGui::Text("Frame %.3f ms (%.1f FPS)", frame_ms, fps);
    ImGui::Checkbox("Pause rotation", &pause_rotation);
    ImGui::SliderFloat("Rotation speed", &rotation_speed, 0.0f, 3.0f, "%.2f");
    ImGui::ColorEdit3("Clear color", clear_color);
    ImGui::Checkbox("Show ImGui demo", &show_demo);
    ImGui::End();
    if (show_demo) {
        ImGui::ShowDemoWindow(&show_demo);
    }

    ImGui::Begin("Viewport");
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const int target_w = static_cast<int>(avail.x);
    const int target_h = static_cast<int>(avail.y);
    if (target_w > 0 && target_h > 0) {
        viewport_width = target_w;
        viewport_height = target_h;
        render_system.ensure_render_target(target_w, target_h);
        const sg_view view = render_system.render_target_view();
        if (view.id) {
            ImGui::Image(simgui_imtextureid(view), avail, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        } else {
            ImGui::TextUnformatted("Waiting for render target...");
        }
    } else {
        ImGui::TextUnformatted("Viewport too small.");
    }
    ImGui::End();
}

void ExampleApplication::on_cleanup() {
    render_system.shutdown();
    sg_shutdown();
}

} // namespace Lite
