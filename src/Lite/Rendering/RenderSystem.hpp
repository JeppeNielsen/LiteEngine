#pragma once

#include "SokolGfx.hpp"

#include <entt/entt.hpp>

namespace Lite {

class RenderSystem {
public:
    void init(const sg_environment& env);
    void shutdown();

    void begin_frame(sg_swapchain swapchain, const float clear_color[3]);
    void begin_offscreen_frame(const float clear_color[3]);
    void render(entt::registry& registry, float aspect);
    void end_frame();
    void ensure_render_target(int width, int height);
    sg_view render_target_view() const;

private:
    void destroy_render_target();

    sg_environment env{};
    sg_pipeline pipeline{};
    sg_shader shader{};
    sg_pass_action pass_action{};
    sg_image target_color{};
    sg_image target_depth{};
    sg_image target_resolve{};
    sg_view target_color_attachment{};
    sg_view target_depth_attachment{};
    sg_view target_resolve_attachment{};
    sg_view target_texture_view{};
    int target_width = 0;
    int target_height = 0;
};

} // namespace Lite
