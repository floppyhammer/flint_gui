#ifndef FLINT_RENDER_IMAGE_H
#define FLINT_RENDER_IMAGE_H

#include <pathfinder/prelude.h>
#include <src/servers/render_server.h>

#include "../common/geometry.h"
#include "image.h"

namespace Flint {

class RenderImage : public Image {
public:
    explicit RenderImage(Vec2I _size) : Image(_size) {
        size = _size;

        Pathfinder::TextureDescriptor desc = {
            size,
            Pathfinder::TextureFormat::Rgba8Unorm,
        };

        texture_ = RenderServer::get_singleton()->device_->create_texture(desc, "render image");
    }

    std::shared_ptr<Pathfinder::Texture> get_texture() const {
        return texture_;
    }

protected:
    std::shared_ptr<Pathfinder::Texture> texture_;
};

} // namespace Flint

#endif // FLINT_RENDER_IMAGE_H
