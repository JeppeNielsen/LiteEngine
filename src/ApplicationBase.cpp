#include "ApplicationBase.hpp"

#include "sokol_log.h"

ApplicationBase* ApplicationBase::active_app_ = nullptr;

AppConfig ApplicationBase::config() const {
    return AppConfig{};
}

void ApplicationBase::init() {
    on_init();
}

void ApplicationBase::frame() {
    on_frame();
}

void ApplicationBase::cleanup() {
    on_cleanup();
}

void ApplicationBase::on_event(const sapp_event* ev) {
    on_event_impl(ev);
}

void ApplicationBase::on_cleanup() {}

void ApplicationBase::on_event_impl(const sapp_event* /*ev*/) {}

sapp_desc ApplicationBase::create_desc(ApplicationBase& app) {
    active_app_ = &app;

    const AppConfig cfg = app.config();

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
    if (active_app_) {
        active_app_->init();
    }
}

void ApplicationBase::frame_cb() {
    if (active_app_) {
        active_app_->frame();
    }
}

void ApplicationBase::cleanup_cb() {
    if (active_app_) {
        active_app_->cleanup();
        active_app_ = nullptr;
    }
}

void ApplicationBase::event_cb(const sapp_event* ev) {
    if (active_app_) {
        active_app_->on_event(ev);
    }
}
