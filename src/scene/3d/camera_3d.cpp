#include "camera_3d.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "scene/sub_viewport.h"

namespace Flint {
void Camera3D::look_at(const glm::vec3 &target) {
    auto distance = target - position;
    // direction = distance / glm::length(distance);
}

void Camera3D::make_current(bool p_enable) {
    current = p_enable;

    Node *viewport_node = get_viewport();

    if (viewport_node) {
        auto viewport = dynamic_cast<SubViewport *>(viewport_node);
    }
}

void Camera3D::update(double delta) {
    auto facing = up;
    facing = glm::rotate(facing, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    facing = glm::rotate(facing, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    facing = glm::rotate(facing, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    // FIXME: Modify center to camera direction.
    view_matrix = glm::lookAt(position, position + facing, up);
}

glm::mat4 Camera3D::get_view_matrix() const {
    return view_matrix;
}

glm::vec3 Camera3D::get_up_direction() const {
    return up;
}
} // namespace Flint
