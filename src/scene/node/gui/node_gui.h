#ifndef FLINT_NODE_GUI_H
#define FLINT_NODE_GUI_H

#include <vector>

#include "../node.h"
#include "../../../common/vec2.h"
#include "../../../resources/mesh.h"
#include "../../../render/render_server.h"
#include "../../../render/mvp_buffer.h"

namespace Flint {
    class NodeGui : public Node {
    public:
        NodeGui();

        ~NodeGui() = default;

        void _update(double delta) override;

        Vec2<float> position{0};
        Vec2<float> size{128};
        Vec2<float> scale{1};
        Vec2<float> pivot_offset{0}; // Top-left as the default pivot.
        float rotation = 0;

    protected:
        void update(double delta) override;

        void update_mvp();

        std::shared_ptr<Mesh2d> mesh;

        std::shared_ptr<Mesh2dDescSet> desc_set;

        std::shared_ptr<Material2d> material;

        std::shared_ptr<MvpBuffer> mvp_buffer;

        void init_default_mesh();
    };
}

#endif //FLINT_NODE_GUI_H