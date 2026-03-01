#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lite {

glm::mat4 Transform::matrix() const {
    const glm::mat4 rot_x = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 rot_y = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 rot_z = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 rotation_matrix = rot_z * rot_y * rot_x;
    const glm::mat4 translated = glm::translate(glm::mat4(1.0f), position) * rotation_matrix;
    return glm::scale(translated, scale);
}

} // namespace Lite
