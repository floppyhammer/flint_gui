#include "camera2d.h"

#include "../world.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace Flint::Scene {

void Camera2d::set_active(bool new_value) {
    active = new_value;
}

void Camera2d::update(double delta) {
}

Vec2I Camera2d::get_extent() const {
    return {};
}

std::shared_ptr<ImageTexture> Camera2d::get_texture() const {
    return subview->texture;
}

} // namespace Flint
