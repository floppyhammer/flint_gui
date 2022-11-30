#ifndef FLINT_NODE_3D_H
#define FLINT_NODE_3D_H

#include "glm/glm.hpp"
#include "render/mvp.h"
#include "render/render_server.h"
#include "resources/surface.h"
#include "scene/node.h"
//#include <Eigen/Geometry>

namespace Flint {
class Node3D : public Node {
public:
    Node3D();

    ~Node3D();

    // Transform.
    // ------------------------------------------
    glm::vec3 position{0};

    glm::vec3 rotation{0};
    // Eigen::Quaternionf quaternion;

    glm::vec3 scale{1};
    // ------------------------------------------

protected:
    void update(double delta) override;

    void update_mvp();

    MvpPushConstant push_constant;
};
} // namespace Flint

#endif // FLINT_NODE_3D_H
