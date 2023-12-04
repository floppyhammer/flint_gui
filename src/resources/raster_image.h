#ifndef FLINT_RASTER_IMAGE_H
#define FLINT_RASTER_IMAGE_H

#include <memory>

#include "../render/base.h"
#include "image.h"

namespace Flint {

class RasterImage final : public Image {
public:
    RasterImage();

    explicit RasterImage(const std::string &path);

    std::shared_ptr<Pathfinder::Image> image_data;
};

} // namespace Flint

#endif // FLINT_RASTER_IMAGE_H
