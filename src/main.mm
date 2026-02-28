#define SOKOL_IMPL
#define SOKOL_APP_IMPL
#define SOKOL_GFX_IMPL
#define SOKOL_GLUE_IMPL
#define SOKOL_LOG_IMPL
#define SOKOL_IMGUI_IMPL
#define SOKOL_GLCORE

#include "SokolApp.hpp"
#include "Rendering/SokolGfx.hpp"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "imgui.h"
#include "sokol_imgui.h"

#include "ExampleApplication.hpp"

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    static ExampleApplication app;
    return ApplicationBase::create_desc(app);
}
