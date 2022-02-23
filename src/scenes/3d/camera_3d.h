#ifndef FLINT_CAMERA_3D_H
#define FLINT_CAMERA_3D_H

#include "node_3d.h"

namespace Flint {
    class Camera3D : public Node3D {
    public:
        Vec3<float> direction = Vec3<float>(0);

        /**
         * Make the camera look at a point.
         * @param target Where to look.
         */
        void lookAt(const Vec3<float> &target);

        /**
         * Set as the current camera of the viewport.
         */
        void makeCurrent();
    };
}

#endif //FLINT_CAMERA_3D_H
