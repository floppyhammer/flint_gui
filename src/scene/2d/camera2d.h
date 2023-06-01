#ifndef FLINT_CAMERA2D_H
#define FLINT_CAMERA2D_H

#include "../../common/geometry.h"
#include "node2d.h"
#include "render/subview.h"

namespace Flint {

class Camera2d : public Node2d {
public:
    explicit Camera2d(Vec2I view_size) {
        subview = std::make_shared<Subview>(view_size);
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

#endif // FLINT_CAMERA2D_H
