#pragma once

#include <glm/glm.hpp>

namespace Lite {

struct Camera {
    float fov_degrees = 60.0f;
    float near_plane = 0.01f;
    float far_plane = 100.0f;

    glm::mat4 projection(float aspect) const;
};

} // namespace Lite
