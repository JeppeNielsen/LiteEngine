#pragma once

#include "SokolGfx.hpp"

#include <entt/entt.hpp>

class RenderSystem {
public:
    void init(const sg_environment& env);
    void shutdown();

    void begin_frame(sg_swapchain swapchain, const float clear_color[3]);
    void render(entt::registry& registry, float aspect);
    void end_frame();

private:
    sg_environment env_{};
    sg_pipeline pipeline_{};
    sg_shader shader_{};
    sg_pass_action pass_action_{};
};
