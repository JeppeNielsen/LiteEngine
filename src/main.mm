#define SOKOL_IMPL
#define SOKOL_APP_IMPL
#define SOKOL_GFX_IMPL
#define SOKOL_GLUE_IMPL
#define SOKOL_LOG_IMPL
#define SOKOL_IMGUI_IMPL
#define SOKOL_GLCORE

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "imgui.h"
#include "sokol_imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    float x, y, z;
    float r, g, b, a;
} vertex_t;

typedef struct {
    glm::mat4 mvp;
} vs_params_t;

static sg_environment g_env;
static sg_pipeline g_pip;
static sg_bindings g_bind;
static sg_pass_action pass_action;
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

    const vertex_t vertices[] = {
        {  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

        { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },

        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
    };

    const uint16_t indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage.vertex_buffer = true;
    vbuf_desc.usage.immutable = true;
    vbuf_desc.label = "cube-vertices";
    g_bind.vertex_buffers[0] = sg_make_buffer(vbuf_desc);

    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.usage.index_buffer = true;
    ibuf_desc.usage.immutable = true;
    ibuf_desc.label = "cube-indices";
    g_bind.index_buffer = sg_make_buffer(ibuf_desc);

    sg_shader_desc shd_desc = {};
    shd_desc.attrs[0].glsl_name = "position";
    shd_desc.attrs[1].glsl_name = "color0";
    shd_desc.vertex_func.source =
        "#version 330\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "uniform mat4 mvp;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    color = color0;\n"
        "}\n";
    shd_desc.fragment_func.source =
        "#version 330\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = color;\n"
        "}\n";

    sg_shader_uniform_block* ub = &shd_desc.uniform_blocks[0];
    ub->stage = SG_SHADERSTAGE_VERTEX;
    ub->size = sizeof(vs_params_t);
    ub->layout = SG_UNIFORMLAYOUT_NATIVE;
    ub->glsl_uniforms[0].glsl_name = "mvp";
    ub->glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;
    ub->glsl_uniforms[0].array_count = 1;

    sg_shader shd = sg_make_shader(shd_desc);

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.layout.buffers[0].stride = (int)sizeof(vertex_t);
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[0].offset = (int)offsetof(vertex_t, x);
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;
    pip_desc.layout.attrs[1].offset = (int)offsetof(vertex_t, r);
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pip_desc.cull_mode = SG_CULLMODE_BACK;
    pip_desc.face_winding = SG_FACEWINDING_CCW;
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = true;
    pip_desc.depth.pixel_format = g_env.defaults.depth_format;
    pip_desc.color_count = 1;
    pip_desc.colors[0].pixel_format = g_env.defaults.color_format;
    pip_desc.sample_count = g_env.defaults.sample_count;
    g_pip = sg_make_pipeline(&pip_desc);

    pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.colors[0].clear_value = { g_clear_color[0], g_clear_color[1], g_clear_color[2], 1.0f };
    pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    pass_action.depth.clear_value = 1.0f;
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

    const float aspect = (float)sapp_width() / (float)sapp_height();
    const glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.01f, 100.0f);
    const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
    const glm::mat4 rot_x = glm::rotate(glm::mat4(1.0f), (float)g_time_sec * 0.7f, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 rot_y = glm::rotate(glm::mat4(1.0f), (float)g_time_sec * 1.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 model = rot_y * rot_x;
    const glm::mat4 mvp = proj * view * model;
    const vs_params_t vs_params = { mvp };

    sg_pass pass = {};
    pass.action = pass_action;
    pass.action.colors[0].clear_value = { g_clear_color[0], g_clear_color[1], g_clear_color[2], 1.0f };
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);
    sg_apply_pipeline(g_pip);
    sg_apply_bindings(&g_bind);
    sg_apply_uniforms(0, SG_RANGE(vs_params));
    sg_draw(0, 36, 1);
    simgui_render();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
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
