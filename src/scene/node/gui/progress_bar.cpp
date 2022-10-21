#include "progress_bar.h"

#include "../../../common/geometry.h"

namespace Flint {
ProgressBar::ProgressBar() {
    type = NodeType::ProgressBar;

    theme_progress = std::optional(StyleBox());
    theme_progress.value().bg_color = ColorU(199, 84, 80);

    theme_bg = std::optional(StyleBox());

    theme_fg = std::optional(StyleBox());
    theme_fg->bg_color = ColorU();
    theme_fg->border_color = ColorU(150, 150, 150);
    theme_fg->border_width = 2;

    // Don't add the label as a child since it's not a normal node but part of the button.
    label = std::make_shared<Label>("%");
    label->set_mouse_filter(MouseFilter::Ignore);
    label->set_horizontal_alignment(Alignment::Center);
    label->set_vertical_alignment(Alignment::Center);
    label->set_parent(this);

    size = label->calculate_minimum_size();
    label->set_size(size);
}

Vec2<float> ProgressBar::calculate_minimum_size() const {
    return label->calculate_minimum_size().max(minimum_size);
}

void ProgressBar::update(double dt) {
    Control::update(dt);

    set_value(Engine::get_singleton()->get_fps());
}

void ProgressBar::draw(VkCommandBuffer p_command_buffer) {
    auto vector_server = VectorServer::get_singleton();

    if (theme_bg.has_value()) {
        vector_server->draw_style_box(theme_bg.value(), get_global_position(), size);
    }

    if (theme_progress.has_value()) {
        vector_server->draw_style_box(theme_progress.value(), get_global_position(), {size.x * ratio, size.y});
    }

    if (theme_fg.has_value()) {
        vector_server->draw_style_box(theme_fg.value(), get_global_position(), size);
    }

    label->draw(p_command_buffer);
}

void ProgressBar::set_position(Vec2<float> p_position) {
    position = p_position;
}

void ProgressBar::set_size(Vec2<float> p_size) {
    size = p_size;
    label->set_size(p_size);
}

void ProgressBar::set_value(float p_value) {
    value = std::clamp(p_value, min_value, max_value);

    ratio = (value - min_value) / (max_value - min_value);

    label->set_text(std::to_string((int)round(ratio * 100)) + "%");
    label->update(0);
}

void ProgressBar::value_changed() {
    for (auto &callback : on_value_changed) {
        callback();
    }
}

void ProgressBar::connect_signal(const std::string &signal, std::function<void()> callback) {
    if (signal == "on_value_changed") {
        on_value_changed.push_back(callback);
    }
}
} // namespace Flint
