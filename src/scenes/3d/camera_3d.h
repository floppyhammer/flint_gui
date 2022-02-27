#ifndef FLINT_CAMERA_3D_H
#define FLINT_CAMERA_3D_H

#include "node_3d.h"

namespace Flint {
    class Camera3D : public Node3D {
    public:
        // FIXME: Should use rotation directly instead of adding a new member.
        /// Vision direction. Position + Direction = Where to look.
        glm::vec3 direction = glm::vec3(0);

        /// Defining the world's upwards direction, which always point towards positive Y.
        glm::vec3 up = glm::vec3(0, 0, 1);

        /**
         * Make the camera look at a point.
         * @param target Where to look.
         */
        void look_at(const glm::vec3 &target);

        /**
         * Set as the current camera of the viewport.
         */
        void make_current(bool p_enable);

    protected:
        bool current = true;
    };
}

#endif //FLINT_CAMERA_3D_H
