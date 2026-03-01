#include "ExampleApplication.hpp"

#include "Rendering/Camera.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Renderable.hpp"
#include "Rendering/SokolGfx.hpp"
#include "Rendering/Transform.hpp"

#include "SokolApp.hpp"
#include "imgui.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include <glm/glm.hpp>

AppConfig ExampleApplication::config() const {
    AppConfig cfg = ApplicationBase::config();
    cfg.title = "Sokol + Premake (macOS)";
    return cfg;
}

void ExampleApplication::on_init() {
    sg_desc desc = {};
    desc.environment = environment();
    desc.logger.func = slog_func;
    sg_setup(&desc);

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

    if (registry.valid(cube_entity)) {
        Transform& cube_transform = registry.get<Transform>(cube_entity);
        cube_transform.rotation = glm::vec3(
            static_cast<float>(time_sec) * 0.7f,
            static_cast<float>(time_sec) * 1.1f,
            0.0f);
    }

    const float aspect = static_cast<float>(sapp_width()) / static_cast<float>(sapp_height());
    render_system.begin_frame(sglue_swapchain(), clear_color);
    render_system.render(registry, aspect);
    gui().render();
    render_system.end_frame();
}

void ExampleApplication::on_cleanup() {
    render_system.shutdown();
    sg_shutdown();
}
