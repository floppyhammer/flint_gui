#include "tab_container.h"

namespace Flint {

TabContainer::TabContainer() {
    type = NodeType::TabContainer;

    theme_panel = std::make_optional<StyleBox>();
    theme_panel.value().corner_radius = 0;
    theme_panel.value().border_width = 0;
    theme_panel.value().bg_color = ColorU(48, 48, 48);

    theme_button_panel = std::make_optional<StyleBox>();
    theme_button_panel.value().corner_radius = 0;
    theme_button_panel.value().border_width = 0;
    theme_button_panel.value().bg_color = ColorU(29, 29, 29);

    button_container = std::make_shared<HStackContainer>();
    button_container->set_parent(this);

    for (int i = 0; i < 3; i++) {
        auto button = std::make_shared<Button>();
        button->set_text("Tab " + std::to_string(i));
        auto callback = [this, i] { this->set_current_tab(i); };
        button->connect_signal("pressed", callback);
        button->set_toggle_mode(true);

        button->theme_normal.value().border_width = 0;
        button->theme_normal.value().corner_radius = 0;
        button->theme_normal.value().bg_color = ColorU(29, 29, 29);

        button->theme_hovered.value().border_width = 0;
        button->theme_hovered.value().corner_radius = 0;
        button->theme_hovered.value().bg_color = ColorU(35, 35, 35);

        button->theme_pressed.value().border_width = 0;
        button->theme_pressed.value().corner_radius = 0;
        button->theme_pressed.value().bg_color = ColorU(48, 48, 48);

        button_container->add_child(button);

        tab_button_group.add_button(button);

//        auto close_button = std::make_shared<Button>();
//        close_button->set_text("X");
//        close_button->set_size({24, 24});
//        button->add_child(close_button);
//        close_button->set_anchor_flag(AnchorFlag::CenterRight);
    }

    set_debug_mode(true);
}

void TabContainer::adjust_layout() {
    Container::adjust_layout();

    auto tab_button_height = button_container->calculate_minimum_size().y;

    for (int i = 0; i < children.size(); i++) {
        children[i]->set_visibility(false);
        if (i == current_tab) {
            children[i]->set_visibility(true);
        }

        if (children[i]->is_gui_node()) {
            auto cast_child = dynamic_cast<Control *>(children[i].get());
            cast_child->set_position({0, tab_button_height});
        }
    }

    button_container->set_size({size.x, 0});
    button_container->adjust_layout();
}

void TabContainer::update(double dt) {
    Control::update(dt);

    adjust_layout();

    button_container->propagate_update(dt);

    tab_button_group.update();
}

Vec2F TabContainer::calculate_minimum_size() const {
    Vec2F min_size;

    uint32_t visible_child_count = 0;

    // Add every child's minimum size.
    for (auto &child : children) {
        if (child->is_gui_node()) {
            auto cast_child = dynamic_cast<Control *>(child.get());
            auto child_min_size = cast_child->calculate_minimum_size();
        }
    }

    return min_size.max(minimum_size);
}

void TabContainer::set_current_tab(int32_t tab) {
    current_tab = tab;
}

void TabContainer::draw(VkCommandBuffer cmd_buffer) {
    Control::draw(cmd_buffer);

    auto vs = VectorServer::get_singleton();

    auto global_pos = get_global_position();

    auto tab_button_height = button_container->calculate_minimum_size().y;

    vs->draw_style_box(theme_button_panel.value(), global_pos, {size.x, tab_button_height});
    vs->draw_style_box(
        theme_panel.value(), global_pos + Vec2F(0, tab_button_height), size - Vec2F(0, tab_button_height));

    //    vs->draw_line(
    //        global_pos + Vec2F{0, tab_button_height}, global_pos + Vec2F{size.x, tab_button_height}, 1,
    //        ColorU::white());

    button_container->propagate_draw(cmd_buffer);
}

void TabContainer::input(InputEvent &event) {
    button_container->propagate_input(event);
}

} // namespace Flint
