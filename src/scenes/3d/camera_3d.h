#ifndef FLINT_CAMERA_3D_H
#define FLINT_CAMERA_3D_H

#include "node_3d.h"

#include <glm/glm.hpp>

namespace Flint {
    class Camera3D : public Node3D {
    public:
        /**
         * Make the camera look at a point.
         * @param target Where to look.
         */
        void look_at(const glm::vec3 &target);

        /**
         * Set as the current camera of the viewport.
         */
        void make_current(bool p_enable);

        [[nodiscard]] glm::mat4 get_view_matrix() const;

        glm::vec3 get_up_direction() const;

    protected:
        void _update(double delta) override;

        void update(double delta) override;

        bool current = true;

        glm::mat4 view_matrix{};

        /// Defining the world's upwards direction, which always point towards positive Y.
        glm::vec3 up = glm::vec3(0, 0, 1);
    };
}

#endif //FLINT_CAMERA_3D_H
