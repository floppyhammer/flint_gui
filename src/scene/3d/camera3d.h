#ifndef FLINT_CAMERA3D_H
#define FLINT_CAMERA3D_H

#include "../../common/geometry.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "node3d.h"
#include "render/subview.h"

namespace Flint {

class Camera3d : public Node3d {
public:
    Camera3d(Vec2I view_size) {
        subview = std::make_shared<Subview>(view_size);
    }

    /**
     * Make the camera look at a point.
     * @param target Where to look.
     */
    void look_at(const glm::vec3 &target);

    /**
     * Set as the current camera of the viewport.
     */
    void set_active(bool new_value);

    [[nodiscard]] glm::mat4 get_view_matrix() const;

    glm::vec3 get_up_direction() const;

    std::shared_ptr<ImageTexture> get_texture() const;

    Vec2I get_extent() const;

    glm::mat4 get_view() const {
        // TODO: change hardcoded center.
        return glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), up);
    }

    glm::mat4 get_projection() const {
        auto proj =
            glm::perspective(glm::radians(fov), (float)subview->extent.x / (float)subview->extent.y, z_near, z_far);

        // GLM was originally designed for OpenGL,
        // where the Y coordinate of the clip coordinates is inverted.
        proj[1][1] *= -1;

        return proj;
    }

    std::shared_ptr<Subview> subview;

    ColorU clear_color = {25, 50, 75};

    float fov = 45.0;

    float z_near = 0.1;
    float z_far = 10.0;

protected:
    void update(double dt) override;

    bool active = true;

    glm::mat4 view_matrix{};

    glm::vec3 direction{};

    /// Defining the world's upwards direction, which always point towards positive Y.
    glm::vec3 up = glm::vec3(0, 0, 1);
};

} // namespace Flint

#endif // FLINT_CAMERA3D_H
