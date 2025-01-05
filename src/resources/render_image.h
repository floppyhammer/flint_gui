#ifndef FLINT_RENDER_IMAGE_H
#define FLINT_RENDER_IMAGE_H

#include <pathfinder/prelude.h>
#include <src/servers/vector_server.h>

#include <memory>

#include "../common/geometry.h"
#include "image.h"

namespace Flint {

class RenderImage : public Image {
public:
    explicit RenderImage(Vec2I _size) {
        size = _size;
    }

    // Should run before other logic during a frame.
    void reclaim_render_target() {
        render_target_id = VectorServer::get_singleton()->create_render_target(size, "render image");
    }

    Pathfinder::RenderTargetId get_render_target() const {
        return render_target_id;
    }

protected:
    Pathfinder::RenderTargetId render_target_id;
};

} // namespace Flint

#endif // FLINT_RENDER_IMAGE_H
