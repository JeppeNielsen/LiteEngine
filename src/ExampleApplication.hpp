#pragma once

#include "ApplicationBase.hpp"
#include "Rendering/RenderSystem.hpp"

#include <entt/entt.hpp>

class ExampleApplication final : public ApplicationBase {
public:
    AppConfig config() const override;

protected:
    void on_init() override;
    void on_frame() override;
    void on_cleanup() override;
    void on_event_impl(const sapp_event* ev) override;

private:
    sg_environment env_{};
    entt::registry registry_{};
    RenderSystem render_system_{};
    entt::entity camera_entity_ = entt::null;
    entt::entity cube_entity_ = entt::null;
    double time_sec_ = 0.0;
    bool pause_rotation_ = false;
    bool show_demo_ = false;
    float rotation_speed_ = 1.0f;
    float clear_color_[3] = { 0.10f, 0.12f, 0.18f };
    const char* imgui_font_path_ = "Assets/LucidaG.ttf";
};
