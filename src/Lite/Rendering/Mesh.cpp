#include "Mesh.hpp"

#include <cstdint>

namespace Lite {

Mesh CreateCubeMesh() {
    const MeshVertex vertices[] = {
        {  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

        { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f },

        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },

        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f },
    };

    const uint16_t indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    Mesh mesh{};
    mesh.index_count = static_cast<int>(sizeof(indices) / sizeof(indices[0]));

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage.vertex_buffer = true;
    vbuf_desc.usage.immutable = true;
    vbuf_desc.label = "cube-vertices";
    mesh.bindings.vertex_buffers[0] = sg_make_buffer(vbuf_desc);

    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.usage.index_buffer = true;
    ibuf_desc.usage.immutable = true;
    ibuf_desc.label = "cube-indices";
    mesh.bindings.index_buffer = sg_make_buffer(ibuf_desc);

    return mesh;
}

} // namespace Lite
