#include "RenderSystem.hpp"

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <cstddef>

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
    shd_desc.vertex_func.source =
        "#version 330\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "uniform mat4 mvp;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    color = color0;\n"
        "}\n";
    shd_desc.fragment_func.source =
        "#version 330\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = color;\n"
        "}\n";

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
    sg_commit();
}
