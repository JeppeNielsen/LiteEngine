#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lite {

glm::mat4 Camera::projection(float aspect) const {
    return glm::perspective(glm::radians(fov_degrees), aspect, near_plane, far_plane);
}

} // namespace Lite
