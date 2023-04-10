#ifndef FLINT_NODE3D_H
#define FLINT_NODE3D_H

#include "../../render/mvp.h"
#include "../../resources/surface.h"
#include "../node.h"
#include "glm/glm.hpp"

namespace Flint::Scene {

class Node3d : public Node {
public:
    Node3d();

    // Transform.
    // ------------------------------------------
    glm::vec3 position{0};

    glm::vec3 rotation{0};
    // Eigen::Quaternionf quaternion;

    glm::vec3 scale{1};
    // ------------------------------------------

protected:
    void update(double dt) override;

    void update_mvp();

    MvpPushConstant push_constant;
};

} // namespace Flint

#endif // FLINT_NODE3D_H
