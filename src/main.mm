#define SOKOL_IMPL
#define SOKOL_APP_IMPL
#define SOKOL_GFX_IMPL
#define SOKOL_GLUE_IMPL
#define SOKOL_LOG_IMPL
#define SOKOL_IMGUI_IMPL
#define SOKOL_GLCORE

#include "sokol_app.h"
#include "Rendering/SokolGfx.hpp"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "imgui.h"
#include "sokol_imgui.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stddef.h>
#include <stdint.h>

#include "Rendering/Camera.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Renderable.hpp"
#include "Rendering/RenderSystem.hpp"
#include "Rendering/Transform.hpp"

static sg_environment g_env;
static entt::registry g_registry;
static RenderSystem g_render_system;
static entt::entity g_camera_entity = entt::null;
static entt::entity g_cube_entity = entt::null;
static double g_time_sec = 0.0;
static bool g_pause_rotation = false;
static bool g_show_demo = false;
static float g_rotation_speed = 1.0f;
static float g_clear_color[3] = { 0.10f, 0.12f, 0.18f };
static const char* g_imgui_font_path = "Assets/LucidaG.ttf";

static void init(void) {
    sg_desc desc = {};
    g_env = sglue_environment();
    desc.environment = g_env;
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
    ImFont* font = io.Fonts->AddFontFromFileTTF(g_imgui_font_path, font_size);
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

    g_render_system.init(g_env);

    g_camera_entity = g_registry.create();
    Transform camera_transform{};
    camera_transform.position = glm::vec3(0.0f, 0.0f, 2.5f);
    g_registry.emplace<Transform>(g_camera_entity, camera_transform);
    g_registry.emplace<Camera>(g_camera_entity);

    g_cube_entity = g_registry.create();
    g_registry.emplace<Transform>(g_cube_entity);
    g_registry.emplace<Renderable>(g_cube_entity);
    g_registry.emplace<Mesh>(g_cube_entity, CreateCubeMesh());
}

static void frame(void) {
    const double dt = sapp_frame_duration();
    if (!g_pause_rotation) {
        g_time_sec += dt * (double)g_rotation_speed;
    }

    simgui_frame_desc_t ui_frame = {};
    ui_frame.width = sapp_width();
    ui_frame.height = sapp_height();
    ui_frame.delta_time = (float)dt;
    ui_frame.dpi_scale = sapp_dpi_scale();
    simgui_new_frame(&ui_frame);

    ImGui::Begin("LiteEngine");
    const float fps = ImGui::GetIO().Framerate;
    const float frame_ms = (fps > 0.0f) ? (1000.0f / fps) : 0.0f;
    ImGui::Text("Frame %.3f ms (%.1f FPS)", frame_ms, fps);
    ImGui::Checkbox("Pause rotation", &g_pause_rotation);
    ImGui::SliderFloat("Rotation speed", &g_rotation_speed, 0.0f, 3.0f, "%.2f");
    ImGui::ColorEdit3("Clear color", g_clear_color);
    ImGui::Checkbox("Show ImGui demo", &g_show_demo);
    ImGui::End();
    if (g_show_demo) {
        ImGui::ShowDemoWindow(&g_show_demo);
    }

    if (g_registry.valid(g_cube_entity)) {
        Transform& cube_transform = g_registry.get<Transform>(g_cube_entity);
        cube_transform.rotation = glm::vec3(
            (float)g_time_sec * 0.7f,
            (float)g_time_sec * 1.1f,
            0.0f);
    }

    const float aspect = (float)sapp_width() / (float)sapp_height();
    g_render_system.begin_frame(sglue_swapchain(), g_clear_color);
    g_render_system.render(g_registry, aspect);
    simgui_render();
    g_render_system.end_frame();
}

static void cleanup(void) {
    simgui_shutdown();
    g_render_system.shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    sapp_desc desc = {};
    desc.logger.func = slog_func;
    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.event_cb = event;
    desc.cleanup_cb = cleanup;
    desc.width = 800;
    desc.height = 450;
    desc.window_title = "Sokol + Premake (macOS)";
    return desc;
}
