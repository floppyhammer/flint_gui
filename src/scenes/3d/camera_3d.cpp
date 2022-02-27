#include "camera_3d.h"

namespace Flint {
    void Camera3D::look_at(const glm::vec3 &target) {
        auto distance = target - position;
        direction = distance / glm::length(distance);
    }

    void Camera3D::make_current(bool p_enable) {
        current = p_enable;
    }
}
