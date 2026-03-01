#pragma once

#include "Lite/App/ApplicationBase.hpp"
#include "Lite/Rendering/RenderSystem.hpp"

#include <entt/entt.hpp>

namespace Lite {

class ExampleApplication final : public ApplicationBase {
public:
    AppConfig config() const override;

protected:
    void on_init() override;
    void on_frame(double dt) override;
    void on_gui() override;
    void on_cleanup() override;

private:

    entt::registry registry{};
    RenderSystem render_system{};
    entt::entity camera_entity = entt::null;
    entt::entity cube_entity = entt::null;
    double time_sec = 0.0;
    bool pause_rotation = false;
    bool show_demo = false;
    float rotation_speed = 1.0f;
    float clear_color[3] = { 0.10f, 0.12f, 0.18f };
};

} // namespace Lite
