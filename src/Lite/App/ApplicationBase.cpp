#include "Lite/App/ApplicationBase.hpp"

#include "imgui.h"
#include "sokol_glue.h"
#include "sokol_log.h"

namespace Lite {

ApplicationBase* ApplicationBase::active_app = nullptr;

AppConfig ApplicationBase::config() const {
    return AppConfig{};
}

void ApplicationBase::init() {
    environment_cache = sglue_environment();
    sg_desc desc = {};
    desc.environment = environment_cache;
    desc.logger.func = slog_func;
    sg_setup(&desc);
    on_init();

    if (config_cache.enable_gui) {
        gui_controller.init(config_cache.imgui_font_path);
        gui_initialized = gui_controller.is_initialized();
    }
}

void ApplicationBase::Start() {
    sapp_desc desc = create_desc(*this);
    sapp_run(&desc);
}

void ApplicationBase::frame() {
    const double dt = sapp_frame_duration();
    if (config_cache.enable_gui && gui_initialized) {
        gui_controller.begin_frame(dt);
    }

    if (config_cache.enable_gui && gui_initialized) {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        on_gui();
    }

    on_frame(dt);

    if (config_cache.enable_gui && gui_initialized) {
        gui_controller.render(sglue_swapchain());
    }

    sg_commit();
}

void ApplicationBase::cleanup() {
    if (config_cache.enable_gui && gui_initialized) {
        gui_controller.shutdown();
        gui_initialized = false;
    }
    on_cleanup();
}

void ApplicationBase::on_event(const sapp_event* ev) {
    if (config_cache.enable_gui && gui_initialized) {
        gui_controller.handle_event(ev);
    }
    on_event_impl(ev);
}

void ApplicationBase::on_cleanup() {}

void ApplicationBase::on_gui() {}

void ApplicationBase::on_event_impl(const sapp_event* /*ev*/) {}

GuiController& ApplicationBase::gui() {
    return gui_controller;
}

const GuiController& ApplicationBase::gui() const {
    return gui_controller;
}

const sg_environment& ApplicationBase::environment() const {
    return environment_cache;
}

sapp_desc ApplicationBase::create_desc(ApplicationBase& app) {
    active_app = &app;

    const AppConfig cfg = app.config();
    app.config_cache = cfg;

    sapp_desc desc = {};
    desc.logger.func = slog_func;
    desc.init_cb = init_cb;
    desc.frame_cb = frame_cb;
    desc.event_cb = event_cb;
    desc.cleanup_cb = cleanup_cb;
    desc.width = cfg.width;
    desc.height = cfg.height;
    desc.window_title = cfg.title;
    return desc;
}

void ApplicationBase::init_cb() {
    if (active_app) {
        active_app->init();
    }
}

void ApplicationBase::frame_cb() {
    if (active_app) {
        active_app->frame();
    }
}

void ApplicationBase::cleanup_cb() {
    if (active_app) {
        active_app->cleanup();
        active_app = nullptr;
    }
}

void ApplicationBase::event_cb(const sapp_event* ev) {
    if (active_app) {
        active_app->on_event(ev);
    }
}

} // namespace Lite
