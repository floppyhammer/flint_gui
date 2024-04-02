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

Label::Label() {
    type = NodeType::Label;

    debug_size_box.border_color = ColorU::blue();

    text_ = "Label";

    font = ResourceManager::get_singleton()->load<Font>("../assets/fonts/Arial Unicode MS Font.ttf");
    emoji_font = ResourceManager::get_singleton()->load<Font>("../assets/fonts/EmojiOneColor.otf");

    text_style.color = {163, 163, 163, 255};

    theme_background = DefaultResource::get_singleton()->get_default_theme()->label.styles["background"];
}

void Label::set_text(const std::string &new_text) {
    // Only update glyphs when text has changed.
    if (text_ == new_text || font == nullptr) {
        return;
    }

    text_ = new_text;
    from_utf8(text_, text_u32_);

    need_to_remeasure = true;
}

void Label::insert_text(uint32_t codepint_position, const std::string &new_text) {
    if (new_text.empty()) {
        return;
    }

    assert(codepint_position <= text_u32_.size() && "Codepoint index is out of bounds!");

    std::u32string new_text_u32;
    from_utf8(new_text, new_text_u32);

    text_u32_.insert(codepint_position, new_text_u32);
    text_ = to_utf8(text_u32_);

    need_to_remeasure = true;
}

void Label::remove_text(uint32_t codepint_position, uint32_t count) {
    assert((codepint_position + count) <= text_u32_.size() && "Codepoint index is out of bounds!");

    text_u32_.erase(codepint_position, count);
    text_ = to_utf8(text_u32_);

    need_to_remeasure = true;
}

std::string Label::get_sub_text(uint32_t codepint_position, uint32_t count) const {
    assert((codepint_position + count) <= text_u32_.size() && "Codepoint index is out of bounds!");

    auto subtext_u32 = text_u32_.substr(codepint_position, count);
    auto subtext = to_utf8(subtext_u32);

    return subtext;
}

std::string Label::get_text() const {
    return text_;
}

std::u32string Label::get_text_u32() const {
    return text_u32_;
}

void Label::set_size(Vec2F new_size) {
    if (size == new_size) {
        return;
    }

    size = new_size;
    consider_alignment();
}

void Label::measure() {
    // Get font info.
    int ascent = font->get_ascent();
    int descent = font->get_descent();

    font->get_glyphs(text_, glyphs_, para_ranges);

    // Add emoji data.
    for (auto &glyph : glyphs_) {
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
            const auto &g = glyphs_[i];

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
    // Make sure size has the correct value before using it.
    size = get_effective_minimum_size();

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
    if (!visible_) {
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

    vector_server->draw_glyphs(glyphs_, glyph_positions, text_style, translation, clip_box);

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

void Label::calc_minimum_size() {
    auto min_size = get_text_size();

    // Label has a minimal height even when the text is empty.
    min_size.y = std::max(min_size.y, (float)font->get_size());

    calculated_minimum_size = min_size;
}

Vec2F Label::get_text_size() const {
    return layout_box.is_valid() ? layout_box.size() : Vec2F(0);
}

std::vector<Glyph> &Label::get_glyphs() {
    return glyphs_;
}

std::shared_ptr<Font> Label::get_font() const {
    return font;
}

float Label::get_glyph_right_edge_position(int32_t glyph_index) {
    assert(glyph_index >= 0 && "Invalid glyph index!");

    float pos = 0;

    assert(glyph_index < glyphs_.size() && "Out of bounds glyph index!");

    for (int i = 0; i <= glyph_index; i++) {
        pos += glyphs_[i].x_advance;
    }

    return pos;
}

float Label::get_glyph_left_edge_position(int32_t glyph_index) {
    assert(glyph_index >= 0 && "Invalid glyph index!");

    float pos = 0;

    for (int i = 0; i < glyph_index; i++) {
        pos += glyphs_[i].x_advance;
    }

    return pos;
}

float Label::get_codepoint_left_edge_position(int32_t codepoint_index) {
    assert(codepoint_index >= 0 && "Invalid codepoint index!");

    float pos = 0;

    int32_t current_codepoint = 0;

    for (int i = 0; i < glyphs_.size(); i++) {
        if (current_codepoint == codepoint_index) {
            return pos;
        }
        const auto &glyph = glyphs_[i];
        int32_t glyph_codepoint_count = glyph.codepoints.size();

        for (int j = 0; j < glyph_codepoint_count; j++) {
            pos += glyph.x_advance / glyph_codepoint_count;

            current_codepoint++;
        }
    }

    return 0;
}

float Label::get_codepoint_right_edge_position(int32_t codepoint_index) {
    assert(codepoint_index >= 0 && "Invalid codepoint index!");

    float pos = 0;

    int32_t current_codepoint = 0;

    for (int i = 0; i < glyphs_.size(); i++) {
        const auto &glyph = glyphs_[i];
        int32_t glyph_codepoint_count = glyph.codepoints.size();

        for (int j = 0; j < glyph_codepoint_count; j++) {
            pos += glyph.x_advance / (float)glyph_codepoint_count;

            if (current_codepoint == codepoint_index) {
                return pos;
            }

            current_codepoint++;
        }
    }

    return 0;
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
