#include "label.h"

#include <string>

namespace Flint {
    Label::Label() {
        type = NodeType::Label;

        debug_size_box.border_color = ColorU::red();

        set_font(ResourceManager::get_singleton()->load<Flint::Font>("../assets/unifont-14.0.03.ttf"));

        set_text("Label");
    }

    void Label::set_text(const std::string &p_text) {
        auto ws_text = utf8_to_ws(p_text);

        // Only update glyphs when text has changed.
        if (text == ws_text || font == nullptr) return;

        text = ws_text;

        measure();
    }

    void Label::insert_text(uint32_t position, const std::string &p_text) {
        if (p_text.empty()) return;

        text.insert(position, utf8_to_ws(p_text));

        measure();
    }

    void Label::remove_text(uint32_t position, uint32_t count) {
        if (position == -1) return;

        text.erase(position, count);

        measure();
    }

    std::string Label::get_text() const {
        return ws_to_utf8(text);
    }

    void Label::set_size(Vec2<float> p_size) {
        if (size == p_size) return;

        size = p_size;
        consider_alignment();
    }

    void Label::measure() {
        // Get font info. Get font scaling.
        int ascent, descent, line_gap;
        float scale = font->get_metrics(font_size, ascent, descent, line_gap);

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
                y += font_size;
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
            g.shape.translate(Pathfinder::Vec2<float>(x, font_size + descent + y));
            // --------------------------------

            // Layout box.
            g.layout_box = Rect<float>(x, font_size + descent - ascent + y, x + advance_width * scale,
                                       font_size + y);

            // Update text's layout box.
            layout_box = layout_box.union_rect(g.layout_box);

            // Bbox.
            g.bbox = Rect<float>(x + bounding_box.left, font_size + descent + bounding_box.bottom + y,
                                 x + bounding_box.right, font_size + descent + bounding_box.top + y);

            // Advance x.
            x += roundf(advance_width * scale);

            glyphs.push_back(g);
        }
    }

    void Label::set_font(std::shared_ptr<Font> p_font) {
        if (p_font == nullptr) return;

        font = std::move(p_font);

        if (text.empty()) return;

        measure();
    }

    void Label::consider_alignment() {
        alignment_shift = Vec2<float>(0);

        switch (horizontal_alignment) {
            case Alignment::Begin:
                break;
            case Alignment::Center: {
                alignment_shift.x = size.x * 0.5f - layout_box.center().x;
            }
                break;
            case Alignment::End: {
                alignment_shift.x = size.x - layout_box.width();
            }
                break;
        }

        switch (vertical_alignment) {
            case Alignment::Begin:
                break;
            case Alignment::Center: {
                alignment_shift.y = size.y * 0.5f - layout_box.center().y;
            }
                break;
            case Alignment::End: {
                alignment_shift.y = size.y - layout_box.height();
            }
                break;
        }
    }

    void Label::update(double dt) {
    }

    void Label::set_text_style(float p_size, ColorU p_color, float p_stroke_width, ColorU p_stroke_color) {
        font_size = p_size;
        color = p_color;
        stroke_width = p_stroke_width;
        stroke_color = p_stroke_color;

        measure();
    }

    void Label::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton()->canvas;
        canvas->save_state();

        auto global_position = get_global_position();

        if (theme_background.has_value()){
            theme_background.value().add_to_canvas(global_position, size, canvas);
        }

        auto translation = global_position + alignment_shift;
        auto transform = Pathfinder::Transform2::from_translation({translation.x, translation.y});
        canvas->set_transform(transform);

        canvas->set_shadow_blur(0);

        // Draw glyphs.
        for (Glyph &g: glyphs) {
            // Add fill.
            canvas->set_fill_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU(color.r, color.g, color.b, color.a)));
            canvas->fill_shape(g.shape, Pathfinder::FillRule::Winding);

            // Add stroke if needed.
            canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a)));
            canvas->set_line_width(stroke_width);
            canvas->stroke_shape(g.shape);

            if (debug) {
                canvas->set_line_width(1);

                // Add layout box.
                // --------------------------------
                Pathfinder::Shape layout_shape;
                layout_shape.add_rect({g.layout_box.left, g.layout_box.top, g.layout_box.right, g.layout_box.bottom});

                canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::green()));
                canvas->stroke_shape(layout_shape);
                // --------------------------------

                // Add bbox.
                // --------------------------------
                Pathfinder::Shape bbox_shape;
                bbox_shape.add_rect({g.bbox.left, g.bbox.top, g.bbox.right, g.bbox.bottom});

                canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::red()));
                canvas->stroke_shape(bbox_shape);
                // --------------------------------
            }
        }

        canvas->restore_state();

        Control::draw(p_command_buffer);
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
        auto min_size = get_text_size();
        min_size.y = font_size;

        return min_size.max(minimum_size);
    }

    Vec2<float> Label::get_text_size() const {
        return layout_box.is_valid() ? layout_box.size() : Vec2<float>(0);
    }

    std::vector<Glyph> &Label::get_glyphs() {
        return glyphs;
    }

    float Label::get_font_size() const {
        return font_size;
    }
}
