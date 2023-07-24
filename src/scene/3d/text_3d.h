#ifndef FLINT_SCENE_TEXT_3D_H
#define FLINT_SCENE_TEXT_3D_H

#include "node3d.h"

namespace Flint {

class FontMesh;

class MeshInstance3d;

class Text3d : public Node3d {
public:
    Text3d();

    void set_text(std::string new_text);

protected:
    void update(double dt) override;

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    std::string text;

    std::shared_ptr<MeshInstance3d> instance;

    std::shared_ptr<FontMesh> font_mesh;
};

} // namespace Flint

#endif // FLINT_SCENE_TEXT_3D_H
