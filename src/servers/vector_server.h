#ifndef FLINT_VECTOR_SERVER_H
#define FLINT_VECTOR_SERVER_H

#include <pathfinder.h>

#include "../common/geometry.h"
#include "../resources/image_texture.h"

using Pathfinder::Vec2F;
using Pathfinder::Transform2;
using Pathfinder::ColorU;
using Pathfinder::Rect;

namespace Flint {
    /// A thin wrapper over Pathfinder outline.
    struct VectorPath {
        Pathfinder::Outline outline;
        ColorU fill_color = ColorU();
        ColorU stroke_color = ColorU();
        float stroke_width = 0;
        float opacity = 1;
    };

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

        void init(const std::shared_ptr<Pathfinder::Driver> &driver,
                  float p_canvas_width,
                  float p_canvas_height,
                  const std::vector<char> &area_lut_input);

        void set_render_target(std::shared_ptr<ImageTexture> dest_texture);

        void cleanup();

        void clear_scene();

        void submit();

        void push_scene(const Rect<float> &view_box);

        void pop_scene();

        /// Following paths will be added to the scene attached to the top scene builder.
        std::vector<std::shared_ptr<Pathfinder::SceneBuilder>> scene_stack;

        std::shared_ptr<ImageTexture> get_texture();

        void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

        void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

        void draw_path(const VectorPath &vector_path, Transform2 transform);

        std::shared_ptr<Pathfinder::Canvas> canvas;

        Vec2F default_canvas_size;
    };
}

#endif //FLINT_VECTOR_SERVER_H
