#include "label.h"

#include <string>

namespace Flint {
    void Label::set_text(const std::string &p_text) {
        // Only update glyphs when text has changed.
        if (text == p_text || font == nullptr) return;

        text = p_text;

        need_to_remeasure = true;
    }

    void Label::measure() {
        // Get font info. Get font scaling.
        int ascent, descent, line_gap;
        float scale = font->get_metrics(line_height, ascent, descent, line_gap);

        // Convert text string to utf32 string.
        std::u32string utf32_str(text.begin(), text.end());

        // Offset.
        float x = 0, y = 0;

        glyphs.clear();
        glyphs.reserve(utf32_str.size());

        // Reset text's layout box.
        layout_box = Rect<float>();

        for (char32_t u_codepoint: utf32_str) {
            Glyph g;

            // Set UTF-32 codepoint.
            g.text = u_codepoint;

            // Set glyph index.
            g.index = stbtt_FindGlyphIndex(&font->info, (int) u_codepoint);

            g.x_off = x;
            g.y_off = y;

            if (u_codepoint == '\n') {
                x = 0;
                y += line_height;
                glyphs.push_back(g);
                continue;
            }

            // The horizontal distance to increment (for left-to-right writing) or decrement (for right-to-left writing)
            // the pen position after a glyph has been rendered when processing text.
            // It is always positive for horizontal layouts, and zero for vertical ones.
            int advance_width;

            // The horizontal distance from the current pen position to the glyph's left bbox edge.
            // It is positive for horizontal layouts, and in most cases negative for vertical ones.
            int left_side_bearing;

            stbtt_GetGlyphHMetrics(&font->info, g.index, &advance_width, &left_side_bearing);

            g.advance = (float) advance_width * scale;

            // Get bounding box for character (maybe offset to account for chars that dip above or below the line).
            Rect<int> bounding_box;
            stbtt_GetGlyphBitmapBox(&font->info, g.index, scale, scale,
                                    &bounding_box.left,
                                    &bounding_box.top,
                                    &bounding_box.right,
                                    &bounding_box.bottom);

            // Compute baseline height (different characters have different heights).
            float local_y = ascent + bounding_box.top;

            // Offset
            float byte_offset = x + roundf(left_side_bearing * scale) + (y * size.y);

            // Set glyph shape.
            // --------------------------------
            g.shape = font->get_glyph_shape(g.index);

            g.shape.scale(Pathfinder::Vec2<float>(scale, -scale));
            g.shape.translate(Pathfinder::Vec2<float>(x, line_height + descent + y));
            // --------------------------------

            // Layout box.
            g.layout_box = Rect<float>(x, line_height + descent - ascent + y, x + advance_width * scale,
                                       line_height + y);

            // Update text's layout box.
            layout_box = layout_box.union_rect(g.layout_box);

            // Bbox.
            g.bbox = Rect<float>(x + bounding_box.left, line_height + descent + bounding_box.bottom + y,
                                 x + bounding_box.right, line_height + descent + bounding_box.top + y);

            // Advance x.
            x += roundf(advance_width * scale);

            glyphs.push_back(g);
        }
    }

    void Label::set_font(std::shared_ptr<Font> p_font) {
        if (p_font == nullptr) return;

        font = std::move(p_font);

        if (text.empty()) return;

        need_to_remeasure = true;
    }

    void Label::consider_alignment() {
        Vec2<float> shift;

        switch (horizontal_alignment) {
            case Alignment::Begin:
                break;
            case Alignment::Center: {
                shift.x = size.x * 0.5f - layout_box.center().x;
            }
                break;
            case Alignment::End: {
                shift.x = size.x - layout_box.width();
            }
                break;
        }

        switch (vertical_alignment) {
            case Alignment::Begin:
                break;
            case Alignment::Center: {
                shift.y = size.y * 0.5f - layout_box.center().y;
            }
                break;
            case Alignment::End: {
                shift.y = size.y - layout_box.height();
            }
                break;
        }

        for (Glyph &g: glyphs) {
            g.layout_box += shift;
            g.layout_box += shift;

            g.bbox += shift;
            g.bbox += shift;

            g.shape.translate({shift.x, shift.y});
        }
    }

    void Label::_update(double delta) {
        // Update self.
        update(delta);

        // Update children;
        Node::_update(delta);
    }

    void Label::update(double delta) {
        Control::update(delta);

        if (need_to_remeasure) {
            measure();

            consider_alignment();

            need_to_remeasure = false;
        }
    }

    void Label::set_style(float p_size, ColorU p_color, float p_stroke_width, ColorU p_stroke_color) {
        line_height = p_size;
        color = p_color;
        stroke_width = p_stroke_width;
        stroke_color = p_stroke_color;

        // TODO: Don't remeasure here.
        need_to_remeasure = true;
    }

    void Label::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void Label::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        {
            // Rebuild & draw the style box.
            auto style_box_shape = Pathfinder::Shape();
            style_box_shape.add_rect({0, 0, size.x, size.y}, theme_background.corner_radius);

            auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
            canvas->set_transform(transform);
            canvas->set_fill_paint(Pathfinder::Paint::from_color(theme_background.bg_color));
            canvas->fill_shape(style_box_shape, Pathfinder::FillRule::Winding);

            if (theme_background.border_width > 0) {
                canvas->set_stroke_paint(Pathfinder::Paint::from_color(theme_background.border_color));
                canvas->set_line_width(theme_background.border_width);
                canvas->stroke_shape(style_box_shape);
            }
        }

        // Add stroke.
        for (Glyph &g: glyphs) {
            // Add stroke if needed.
            canvas->set_stroke_paint(Pathfinder::Paint::from_color(ColorU(stroke_color)));
            canvas->set_line_width(stroke_width);
            canvas->stroke_shape(g.shape);
        }

        for (Glyph &g: glyphs) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color(ColorU(color)));
            auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
            canvas->set_transform(transform);
            canvas->fill_shape(g.shape, Pathfinder::FillRule::Winding);

            if (debug) {
                canvas->set_line_width(1);

                // Add layout box.
                // --------------------------------
                Pathfinder::Shape layout_shape;
                layout_shape.add_rect({g.layout_box.left, g.layout_box.top, g.layout_box.right, g.layout_box.bottom});

                canvas->set_stroke_paint(Pathfinder::Paint::from_color(ColorU::green()));
                canvas->stroke_shape(layout_shape);
                // --------------------------------

                // Add bbox.
                // --------------------------------
                Pathfinder::Shape bbox_shape;
                bbox_shape.add_rect({g.bbox.left, g.bbox.top, g.bbox.right, g.bbox.bottom});

                canvas->set_stroke_paint(Pathfinder::Paint::from_color(ColorU::red()));
                canvas->stroke_shape(bbox_shape);
                // --------------------------------
            }
        }
    }

    void Label::set_horizontal_alignment(Alignment alignment) {
        if (horizontal_alignment == alignment) return;

        horizontal_alignment = alignment;

        consider_alignment();
    }

    void Label::set_vertical_alignment(Alignment alignment) {
        if (vertical_alignment == alignment) return;

        vertical_alignment = alignment;

        consider_alignment();
    }

    Vec2<float> Label::calculate_minimum_size() const {
        return layout_box.size();
    }
}
