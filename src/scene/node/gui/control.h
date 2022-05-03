#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../../common/vec2.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/render_server.h"
#include "../../../servers/input_server.h"
#include "../../../servers/vector_server.h"

namespace Flint {
    class Control : public Node {
    public:
        Control();

        ~Control() = default;

        Vec2<float> position{0};
        Vec2<float> size{128};
        Vec2<float> scale{1};
        Vec2<float> pivot_offset{0}; // Top-left as the default pivot.
        float rotation = 0;

        /// Used when being a child of a container.
        Vec2<float> minimum_size{0};

        virtual Vec2<float> calculate_minimum_size() const;

    protected:
        void update(double dt) override;

        void update_mvp();

        void input(std::vector<InputEvent> &input_queue) override;

        Surface2dPushConstant push_constant;
    };
}

#endif //FLINT_CONTROL_H
