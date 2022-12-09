#include "node3d.h"

#include <chrono>

#include "../../common/load_file.h"
#include "../../render/swap_chain.h"
#include "../world.h"
#include "camera3d.h"

namespace Flint {

Node3d::Node3d() {
    type = NodeType::Node3d;
}

void Node3d::update(double dt) {
}

void Node3d::update_mvp() {
    // Prepare MVP data.
    ModelViewProjection mvp{};

    // Determined by model transform.
    mvp.model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
    mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, scale.z));
    mvp.model = glm::rotate(mvp.model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Determined by camera.
    auto world = get_world();
    if (!world) {
        return;
    }

    auto camera = world->current_camera3d;
    if (camera) {
        mvp.view = camera->get_view();
        mvp.proj = camera->get_projection();
    }

    push_constant.mvp = mvp.calculate_mvp();
}

} // namespace Flint
