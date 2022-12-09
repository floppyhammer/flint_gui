#ifndef FLINT_CAMERA3D_H
    #define FLINT_CAMERA_3D_H

#include "../../common/geometry.h"
#include "glm/glm.hpp"
#include "node2d.h"
#include "render/subview.h"

namespace Flint {

class Camera2d : public Node2d {
public:
    Camera2d() {
        subview = std::make_shared<Subview>();
    }

    /**
     * Set as the current camera of the viewport.
     */
    void set_active(bool new_value);

    std::shared_ptr<ImageTexture> get_texture() const;

    Vec2I get_extent() const;

    std::shared_ptr<Subview> subview;

    ColorU clear_color = {25, 50, 75};

protected:
    void update(double delta) override;

    bool active = true;
};

} // namespace Flint

#endif // FLINT_CAMERA3D_H
