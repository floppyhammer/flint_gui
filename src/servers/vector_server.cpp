#include "vector_server.h"

namespace Flint {
    void
    VectorServer::init(const std::shared_ptr<Pathfinder::Driver> &driver, float p_canvas_width, float p_canvas_height,
                       const std::vector<char> &area_lut_input) {
        canvas = std::make_shared<Pathfinder::Canvas>(driver, p_canvas_width, p_canvas_height, area_lut_input);
    }

    void VectorServer::draw_line(Vec2F start, Vec2F end, float width, ColorU color) {
        Pathfinder::Shape shape;
        shape.add_line({start.x, start.y}, {end.x, end.y});

        canvas->save_state();
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
        canvas->set_line_width(width);
        //canvas->set_line_cap(Pathfinder::LineCap::Round);
        canvas->stroke_shape(shape);
        canvas->restore_state();
    }

    void VectorServer::draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color) {
        Pathfinder::Shape shape;
        shape.add_circle({center.x, center.y}, radius);

        canvas->save_state();

        if (fill) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
            canvas->fill_shape(shape, Pathfinder::FillRule::Winding);
        } else if (line_width > 0) {
            canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
            canvas->set_line_width(line_width);
            canvas->stroke_shape(shape);
        }

        canvas->restore_state();
    }

    void VectorServer::draw_vshape(const VShape &vshape, Transform2 transform) {
        canvas->save_state();

        Pathfinder::Transform2 pf_transform;
        pf_transform.vector.x = transform.vector.x;
        pf_transform.vector.y = transform.vector.y;
        for (int i = 0; i < 4; i++) {
            pf_transform.matrix.v[i] = transform.matrix.v[i];
        }

        canvas->set_transform(pf_transform);

        if (!vshape.fill_color.is_transparent()) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color(
                    {vshape.fill_color.r,
                     vshape.fill_color.g,
                     vshape.fill_color.b,
                     vshape.fill_color.a}));
            canvas->fill_shape(vshape.shape, Pathfinder::FillRule::Winding);
        }

        if (vshape.stroke_width > 0) {
            canvas->set_stroke_paint(Pathfinder::Paint::from_color(
                    {vshape.stroke_color.r,
                     vshape.stroke_color.g,
                     vshape.stroke_color.b,
                     vshape.stroke_color.a}));
            canvas->set_line_width(vshape.stroke_width);
            canvas->stroke_shape(vshape.shape);
        }

        canvas->restore_state();
    }
}
