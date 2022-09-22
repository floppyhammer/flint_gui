#ifndef FLINT_RASTER_IMAGE_H
#define FLINT_RASTER_IMAGE_H

#include <cstdint>
#include <vector>

namespace Flint {
/// CPU-only image data.
class RasterImage {
    uint32_t width;
    uint32_t height;
    std::vector<uint32_t> data;
};
} // namespace Flint

#endif // FLINT_IMAGE_H
