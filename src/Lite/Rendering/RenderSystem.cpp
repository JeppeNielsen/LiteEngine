#include "RenderSystem.hpp"

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <cstddef>

namespace Lite {

namespace {
struct VsParams {
    glm::mat4 mvp;
};
}

void RenderSystem::init(const sg_environment& env) {
    this->env = env;

    sg_shader_desc shd_desc = {};
    shd_desc.attrs[0].glsl_name = "position";
    shd_desc.attrs[1].glsl_name = "color0";
#if defined(SOKOL_GLES3)
    const char* vs_src =
        "#version 300 es\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "uniform mat4 mvp;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    color = color0;\n"
        "}\n";
    const char* fs_src =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = color;\n"
        "}\n";
#else
    const char* vs_src =
        "#version 330\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "uniform mat4 mvp;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    color = color0;\n"
        "}\n";
    const char* fs_src =
        "#version 330\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = color;\n"
        "}\n";
#endif
    shd_desc.vertex_func.source = vs_src;
    shd_desc.fragment_func.source = fs_src;

    sg_shader_uniform_block* ub = &shd_desc.uniform_blocks[0];
    ub->stage = SG_SHADERSTAGE_VERTEX;
    ub->size = sizeof(VsParams);
    ub->layout = SG_UNIFORMLAYOUT_NATIVE;
    ub->glsl_uniforms[0].glsl_name = "mvp";
    ub->glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;
    ub->glsl_uniforms[0].array_count = 1;

    shader = sg_make_shader(shd_desc);

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shader;
    pip_desc.layout.buffers[0].stride = static_cast<int>(sizeof(MeshVertex));
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[0].offset = static_cast<int>(offsetof(MeshVertex, x));
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;
    pip_desc.layout.attrs[1].offset = static_cast<int>(offsetof(MeshVertex, r));
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pip_desc.cull_mode = SG_CULLMODE_BACK;
    pip_desc.face_winding = SG_FACEWINDING_CCW;
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = true;
    pip_desc.depth.pixel_format = env.defaults.depth_format;
    pip_desc.color_count = 1;
    pip_desc.colors[0].pixel_format = env.defaults.color_format;
    pip_desc.sample_count = env.defaults.sample_count;
    pipeline = sg_make_pipeline(&pip_desc);

    pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    pass_action.depth.clear_value = 1.0f;
}

void RenderSystem::shutdown() {
    destroy_render_target();
    if (pipeline.id) {
        sg_destroy_pipeline(pipeline);
        pipeline.id = 0;
    }
    if (shader.id) {
        sg_destroy_shader(shader);
        shader.id = 0;
    }
}

void RenderSystem::begin_frame(sg_swapchain swapchain, const float clear_color[3]) {
    sg_pass pass = {};
    pass.action = pass_action;
    pass.action.colors[0].clear_value = { clear_color[0], clear_color[1], clear_color[2], 1.0f };
    pass.swapchain = swapchain;
    sg_begin_pass(&pass);
}

bool RenderSystem::begin_offscreen_frame(const float clear_color[3]) {
    if (!target_color_attachment.id) {
        return false;
    }

    sg_pass pass = {};
    pass.action = pass_action;
    pass.action.colors[0].clear_value = { clear_color[0], clear_color[1], clear_color[2], 1.0f };
    pass.attachments.colors[0] = target_color_attachment;
    if (target_depth_attachment.id) {
        pass.attachments.depth_stencil = target_depth_attachment;
    }
    if (target_resolve_attachment.id) {
        pass.attachments.resolves[0] = target_resolve_attachment;
    }
    sg_begin_pass(&pass);
    return true;
}

void RenderSystem::render(entt::registry& registry, float aspect) {
    auto camera_view = registry.view<const Camera, const Transform>();
    if (camera_view.begin() == camera_view.end()) {
        return;
    }

    const entt::entity camera_entity = *camera_view.begin();
    const Camera& camera = camera_view.get<const Camera>(camera_entity);
    const Transform& camera_transform = camera_view.get<const Transform>(camera_entity);

    const glm::mat4 projection = camera.projection(aspect);
    const glm::mat4 view = glm::inverse(camera_transform.matrix());

    sg_apply_pipeline(pipeline);

    auto render_view = registry.view<const Transform, const Renderable, const Mesh>();
    for (const entt::entity entity : render_view) {
        const Renderable& renderable = render_view.get<const Renderable>(entity);
        if (!renderable.is_visible()) {
            continue;
        }

        const Transform& transform = render_view.get<const Transform>(entity);
        const Mesh& mesh = render_view.get<const Mesh>(entity);

        const glm::mat4 model = transform.matrix();
        const VsParams params = { projection * view * model };

        sg_apply_bindings(&mesh.bindings);
        sg_apply_uniforms(0, SG_RANGE(params));
        sg_draw(0, mesh.index_count, 1);
    }
}

void RenderSystem::end_frame() {
    sg_end_pass();
}

void RenderSystem::ensure_render_target(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    if (width == target_width && height == target_height) {
        return;
    }

    destroy_render_target();
    target_width = width;
    target_height = height;

    sg_pixel_format color_format = env.defaults.color_format;
    sg_pixelformat_info color_info = sg_query_pixelformat(color_format);
    if (!color_info.render) {
        color_format = SG_PIXELFORMAT_RGBA8;
        color_info = sg_query_pixelformat(color_format);
    }

    sg_pixel_format depth_format = env.defaults.depth_format;
    sg_pixelformat_info depth_info = sg_query_pixelformat(depth_format);
    if (!depth_info.depth) {
        depth_format = SG_PIXELFORMAT_DEPTH;
        depth_info = sg_query_pixelformat(depth_format);
        if (!depth_info.depth) {
            depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;
            depth_info = sg_query_pixelformat(depth_format);
        }
    }

    int sample_count = env.defaults.sample_count;
    if (sample_count > 1 && !color_info.msaa) {
        sample_count = 1;
    }

#if defined(__EMSCRIPTEN__)
    if (sample_count > 1) {
        sample_count = 1;
    }
#endif

    sg_image_desc color_desc = {};
    color_desc.width = width;
    color_desc.height = height;
    color_desc.pixel_format = color_format;
    color_desc.sample_count = sample_count;
    color_desc.usage.color_attachment = true;
    color_desc.usage.immutable = true;
    color_desc.label = "lite-render-target-color";
    target_color = sg_make_image(&color_desc);

    sg_image_desc depth_desc = {};
    depth_desc.width = width;
    depth_desc.height = height;
    depth_desc.pixel_format = depth_format;
    depth_desc.sample_count = sample_count;
    depth_desc.usage.depth_stencil_attachment = true;
    depth_desc.usage.immutable = true;
    depth_desc.label = "lite-render-target-depth";
    if (depth_info.depth) {
        target_depth = sg_make_image(&depth_desc);
    }

    if (sample_count > 1) {
        sg_image_desc resolve_desc = {};
        resolve_desc.width = width;
        resolve_desc.height = height;
        resolve_desc.pixel_format = color_format;
        resolve_desc.sample_count = 1;
        resolve_desc.usage.resolve_attachment = true;
        resolve_desc.usage.immutable = true;
        resolve_desc.label = "lite-render-target-resolve";
        target_resolve = sg_make_image(&resolve_desc);
    }

    sg_view_desc color_view_desc = {};
    color_view_desc.color_attachment.image = target_color;
    color_view_desc.color_attachment.mip_level = 0;
    color_view_desc.color_attachment.slice = 0;
    color_view_desc.label = "lite-render-target-color-attachment";
    target_color_attachment = sg_make_view(&color_view_desc);

    sg_view_desc depth_view_desc = {};
    if (target_depth.id) {
        depth_view_desc.depth_stencil_attachment.image = target_depth;
        depth_view_desc.depth_stencil_attachment.mip_level = 0;
        depth_view_desc.depth_stencil_attachment.slice = 0;
        depth_view_desc.label = "lite-render-target-depth-attachment";
        target_depth_attachment = sg_make_view(&depth_view_desc);
    }
    if (target_resolve.id) {
        sg_view_desc resolve_view_desc = {};
        resolve_view_desc.resolve_attachment.image = target_resolve;
        resolve_view_desc.resolve_attachment.mip_level = 0;
        resolve_view_desc.resolve_attachment.slice = 0;
        resolve_view_desc.label = "lite-render-target-resolve-attachment";
        target_resolve_attachment = sg_make_view(&resolve_view_desc);

        sg_view_desc texture_view_desc = {};
        texture_view_desc.texture.image = target_resolve;
        texture_view_desc.texture.mip_levels.base = 0;
        texture_view_desc.texture.mip_levels.count = 1;
        texture_view_desc.texture.slices.base = 0;
        texture_view_desc.texture.slices.count = 1;
        texture_view_desc.label = "lite-render-target-texture";
        target_texture_view = sg_make_view(&texture_view_desc);
    } else {
        sg_view_desc texture_view_desc = {};
        texture_view_desc.texture.image = target_color;
        texture_view_desc.texture.mip_levels.base = 0;
        texture_view_desc.texture.mip_levels.count = 1;
        texture_view_desc.texture.slices.base = 0;
        texture_view_desc.texture.slices.count = 1;
        texture_view_desc.label = "lite-render-target-texture";
        target_texture_view = sg_make_view(&texture_view_desc);
    }
}

sg_view RenderSystem::render_target_view() const {
    return target_texture_view;
}

void RenderSystem::destroy_render_target() {
    if (target_color_attachment.id) {
        sg_destroy_view(target_color_attachment);
        target_color_attachment.id = 0;
    }
    if (target_depth_attachment.id) {
        sg_destroy_view(target_depth_attachment);
        target_depth_attachment.id = 0;
    }
    if (target_resolve_attachment.id) {
        sg_destroy_view(target_resolve_attachment);
        target_resolve_attachment.id = 0;
    }
    if (target_texture_view.id) {
        sg_destroy_view(target_texture_view);
        target_texture_view.id = 0;
    }
    if (target_color.id) {
        sg_destroy_image(target_color);
        target_color.id = 0;
    }
    if (target_depth.id) {
        sg_destroy_image(target_depth);
        target_depth.id = 0;
    }
    if (target_resolve.id) {
        sg_destroy_image(target_resolve);
        target_resolve.id = 0;
    }
    target_width = 0;
    target_height = 0;
}

} // namespace Lite
