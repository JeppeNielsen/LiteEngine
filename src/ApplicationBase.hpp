#pragma once

#include "SokolApp.hpp"

struct AppConfig {
    int width = 800;
    int height = 450;
    const char* title = "Sokol + Premake (macOS)";
};

class ApplicationBase {
public:
    virtual ~ApplicationBase() = default;

    virtual AppConfig config() const;

    void init();
    void frame();
    void cleanup();
    void on_event(const sapp_event* ev);

    static sapp_desc create_desc(ApplicationBase& app);

protected:
    virtual void on_init() = 0;
    virtual void on_frame() = 0;
    virtual void on_cleanup();
    virtual void on_event_impl(const sapp_event* ev);

private:
    static void init_cb();
    static void frame_cb();
    static void cleanup_cb();
    static void event_cb(const sapp_event* ev);

    static ApplicationBase* active_app_;
};
