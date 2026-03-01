#pragma once

#include "SokolGfx.hpp"

namespace Lite {

struct MeshVertex {
    float x, y, z;
    float r, g, b, a;
};

struct Mesh {
    sg_bindings bindings{};
    int index_count = 0;
};

Mesh CreateCubeMesh();

} // namespace Lite
