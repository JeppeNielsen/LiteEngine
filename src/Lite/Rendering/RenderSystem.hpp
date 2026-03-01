#pragma once

#include "SokolGfx.hpp"

#include <entt/entt.hpp>

namespace Lite {

class RenderSystem {
public:
    void init(const sg_environment& env);
    void shutdown();

    void begin_frame(sg_swapchain swapchain, const float clear_color[3]);
    void render(entt::registry& registry, float aspect);
    void end_frame();

private:
    sg_environment env{};
    sg_pipeline pipeline{};
    sg_shader shader{};
    sg_pass_action pass_action{};
};

} // namespace Lite
