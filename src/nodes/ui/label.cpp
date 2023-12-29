#include "label.h"

#include <string>

#include "../../resources/default_resource.h"

using Pathfinder::Transform2;

namespace Flint {

enum class Bidi {
    Auto,
    LeftToRight,
    RightToLeft,
};

Label::Label(const std::string &_text) {
    type = NodeType::Label;

    debug_size_box.border_color = ColorU::red();

    font = ResourceManager::get_singleton()->load<Font>("../assets/fonts/Arial Unicode MS Font.ttf");
    emoji_font = ResourceManager::get_singleton()->load<Font>("../assets/fonts/EmojiOneColor.otf");

    set_text(_text);

    text_style.color = {163, 163, 163, 255};

    theme_background = DefaultResource::get_singleton()->get_default_theme()->label.styles["background"];
}

void Label::set_text(const std::string &new_text) {
    // Only update glyphs when text has changed.
    if (text == new_text || font == nullptr) {
        return;
    }

    text = new_text;

    need_to_remeasure = true;
}

void Label::insert_text(uint32_t position, const std::string &new_text) {
    if (new_text.empty()) {
        return;
    }

    text.insert(position, new_text);

    need_to_remeasure = true;
}

void Label::remove_text(uint32_t position, uint32_t count) {
    if (position == -1) {
        return;
    }

    text.erase(position, count);

    need_to_remeasure = true;
}

std::string Label::get_text() const {
    return text;
}

void Label::set_size(Vec2F p_size) {
    if (size == p_size) {
        return;
    }

    size = p_size;
    consider_alignment();
}

void Label::measure() {
    // Get font info.
    int ascent = font->get_ascent();
    int descent = font->get_descent();

    font->get_glyphs(text, glyphs, para_ranges);

    // Add emoji data.
    for (auto &glyph : glyphs) {
        if (glyph.codepoints.size() == 1) {
            uint16_t glyph_index = emoji_font->find_glyph_index_by_codepoint(glyph.codepoints.front());
            glyph.svg = emoji_font->get_glyph_svg(glyph_index);
            if (!glyph.svg.empty() && glyph.index == 0) {
                glyph.x_advance = font->get_size();
                glyph.box = {0, 0, (float)font->get_size(), (float)font->get_size()};
                glyph.emoji = true;
            }
        }
    }

    // Reset text's layout box.
    layout_box = RectF();

    glyph_positions.clear();

    glyph_boxes.clear();
    character_boxes.clear();

    float line_height = font->get_size();

    float cursor_x = 0;
    float cursor_y = 0;

    // Build layout.
    for (auto &range : para_ranges) {
        for (int i = range.start; i < range.end; i++) {
            const auto &g = glyphs[i];

            // The glyph's layout box in the text's local coordinates.
            // The origin is the top-left corner of the text box.
            RectF glyph_layout_box =
                RectF(cursor_x + g.x_offset, cursor_y + g.y_offset, cursor_x + g.x_advance, cursor_y + line_height);

            glyph_positions.emplace_back(cursor_x + g.x_offset, cursor_y + g.y_offset);

            // The whole text's layout box.
            layout_box = layout_box.union_rect(glyph_layout_box);

            // Advance x.
            cursor_x += g.x_advance;
        }

        cursor_x = 0;
        cursor_y += line_height;
    }
}

void Label::set_font(std::shared_ptr<Font> new_font) {
    if (new_font == nullptr) {
        return;
    }

    font = std::move(new_font);

    need_to_remeasure = true;
}

void Label::consider_alignment() {
    alignment_shift = Vec2F(0);

    switch (horizontal_alignment) {
        case Alignment::Begin:
            break;
        case Alignment::Center: {
            alignment_shift.x = size.x * 0.5f - layout_box.center().x;
        } break;
        case Alignment::End: {
            alignment_shift.x = size.x - layout_box.width();
        } break;
    }

    switch (vertical_alignment) {
        case Alignment::Begin:
            break;
        case Alignment::Center: {
            alignment_shift.y = size.y * 0.5f - layout_box.center().y;
        } break;
        case Alignment::End: {
            alignment_shift.y = size.y - layout_box.height();
        } break;
    }
}

void Label::update(double dt) {
    NodeUi::update(dt);

    if (need_to_remeasure) {
        need_to_remeasure = false;
        measure();
    }

    consider_alignment();
}

void Label::set_text_style(TextStyle _text_style) {
    text_style = _text_style;
}

void Label::draw() {
    if (!visible) {
        return;
    }

    auto global_position = get_global_position();

    auto vector_server = VectorServer::get_singleton();

    vector_server->draw_style_box(theme_background, global_position, size);

    auto baseline_position = Vec2F(0, font->get_ascent());

    auto translation = Transform2::from_translation(global_position + alignment_shift + baseline_position);

    RectF clip_box;
    //    if (clip) {
    //        clip_box = {{}, size};
    //    } else {
    //        clip_box = {{}, calc_minimum_size()};
    //    }

    vector_server->draw_glyphs(glyphs, glyph_positions, text_style, translation, clip_box);

    NodeUi::draw();
}

void Label::set_horizontal_alignment(Alignment alignment) {
    if (horizontal_alignment == alignment) {
        return;
    }

    horizontal_alignment = alignment;

    consider_alignment();
}

void Label::set_vertical_alignment(Alignment alignment) {
    if (vertical_alignment == alignment) {
        return;
    }

    vertical_alignment = alignment;

    consider_alignment();
}

Vec2F Label::calc_minimum_size() const {
    auto min_size = get_text_size();

    // Label has a minimal height even when the text is empty.
    min_size.y = std::max(min_size.y, (float)font->get_size());

    return min_size.max(minimum_size);
}

Vec2F Label::get_text_size() const {
    return layout_box.is_valid() ? layout_box.size() : Vec2F(0);
}

std::vector<Glyph> &Label::get_glyphs() {
    return glyphs;
}

std::shared_ptr<Font> Label::get_font() const {
    return font;
}

float Label::get_glyph_right_edge_position(int32_t glyph_index) {
    if (glyph_index < 0) {
        return 0;
    }

    float pos = 0;

    assert(glyph_index < glyphs.size() && "Out of bounds glyph index!");

    for (int i = 0; i <= glyph_index; i++) {
        pos += glyphs[i].x_advance;
    }

    return pos;
}

float Label::get_glyph_left_edge_position(int32_t glyph_index) {
    if (glyph_index < 0) {
        return 0;
    }

    float pos = 0;

    for (int i = 0; i < glyph_index; i++) {
        pos += glyphs[i].x_advance;
    }

    return pos;
}

float Label::get_position_by_codepoint(uint32_t codepoint_index) {
    float pos = 0;

    // TODO

    return pos;
}
uint32_t Label::get_codepoint_by_position(Vec2F position) {
    return 0;
}

} // namespace Flint
