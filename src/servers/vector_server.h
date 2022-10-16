#ifndef FLINT_VECTOR_SERVER_H
#define FLINT_VECTOR_SERVER_H

#include <pathfinder.h>

#include "../common/geometry.h"
#include "../resources/image_texture.h"
#include "../resources/vector_texture.h"

using Pathfinder::ColorU;
using Pathfinder::Rect;
using Pathfinder::Transform2;
using Pathfinder::Vec2F;

namespace Flint {

/**
 * All visible shapes will be collected by the vector server and drawn at once.
 */
class VectorServer {
public:
    static VectorServer *get_singleton() {
        static VectorServer singleton;
        return &singleton;
    }

    VectorServer() = default;

    void init(const std::shared_ptr<Pathfinder::Driver> &driver, int p_canvas_width, int p_canvas_height);

    void set_render_target(std::shared_ptr<ImageTexture> dest_texture);

    void cleanup();

    void clear_scene();

    void submit();

    /**
     * Enable clipping box, portion of anything drawn afterward
     * outside the clipping box will not show.
     */
    void set_clipping_box(const Rect<float> &box);

    /**
     * Disable the previously set clipping box.
     */
    void unset_clipping_box();

    void push_scene(const Rect<float> &view_box);

    void pop_scene();

    /// Following paths will be added to the scene attached to the top scene builder.
    std::vector<std::shared_ptr<Pathfinder::Scene>> scene_stack;

    std::shared_ptr<ImageTexture> get_texture();

    void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

    void draw_rectangle(const Rect<float> &rect, float line_width, ColorU color);

    void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

    void draw_path(VectorPath &vector_path, Transform2 transform);

    void draw_texture(VectorTexture &texture, Transform2 transform);

    std::shared_ptr<Pathfinder::Canvas> canvas;

    Vec2<int> default_canvas_size;
};

} // namespace Flint

#endif // FLINT_VECTOR_SERVER_H
