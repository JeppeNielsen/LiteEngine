#pragma once

#include "GuiController.hpp"
#include "Rendering/SokolGfx.hpp"

struct AppConfig {
    int width = 800;
    int height = 450;
    const char* title = "Sokol + Premake (macOS)";
    const char* imgui_font_path = "Assets/LucidaG.ttf";
    bool enable_gui = true;
};

class ApplicationBase {
public:
    virtual ~ApplicationBase() = default;

    virtual AppConfig config() const;

    void Start();
    void init();
    void frame();
    void cleanup();
    void on_event(const sapp_event* ev);

protected:
    virtual void on_init() = 0;
    virtual void on_frame(double dt) = 0;
    virtual void on_cleanup();
    virtual void on_event_impl(const sapp_event* ev);

    GuiController& gui();
    const GuiController& gui() const;
    const sg_environment& environment() const;

private:
    static sapp_desc create_desc(ApplicationBase& app);
    static void init_cb();
    static void frame_cb();
    static void cleanup_cb();
    static void event_cb(const sapp_event* ev);

    static ApplicationBase* active_app;
    AppConfig config_cache{};
    bool gui_initialized = false;
    GuiController gui_controller{};
    sg_environment environment_cache{};
};
