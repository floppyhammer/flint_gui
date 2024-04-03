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
    add_embedded_child(button_container);

    for (int i = 0; i < 3; i++) {
        auto button = std::make_shared<Button>();
        button->set_text("Tab " + std::to_string(i));
        auto callback = [this, i] { this->set_current_tab(i); };
        button->connect_signal("pressed", callback);
        button->set_toggle_mode(true);

        button->theme_normal.border_width = 0;
        button->theme_normal.corner_radius = 0;
        button->theme_normal.bg_color = ColorU(29, 29, 29);

        button->theme_hovered.border_width = 0;
        button->theme_hovered.corner_radius = 0;
        button->theme_hovered.bg_color = ColorU(35, 35, 35);

        button->theme_pressed.border_width = 0;
        button->theme_pressed.corner_radius = 0;
        button->theme_pressed.bg_color = ColorU(48, 48, 48);

        button_container->add_child(button);

        tab_button_group.add_button(button);

        //        auto close_button = std::make_shared<Button>();
        //        close_button->set_text("X");
        //        close_button->set_size({24, 24});
        //        button->add_child(close_button);
        //        close_button->set_anchor_flag(AnchorFlag::CenterRight);
    }
}

void TabContainer::adjust_layout() {
    // Adjust own size.
    size = get_effective_minimum_size().max(size);

    auto tab_button_height = button_container->get_effective_minimum_size().y;

    for (int i = 0; i < children.size(); i++) {
        children[i]->set_visibility(i == current_tab);

        if (children[i]->is_ui_node()) {
            auto ui_child = dynamic_cast<NodeUi *>(children[i].get());
            ui_child->set_position({0, tab_button_height});
            ui_child->set_size({size.x, size.y - tab_button_height});
        }
    }

    button_container->set_size({size.x, 0});
}

void TabContainer::update(double dt) {
    Container::update(dt);

    tab_button_group.update();
}

void TabContainer::calc_minimum_size() {
    Vec2F min_size;

    // Find the largest child size.
    for (auto &child : children) {
        // Skip invisible/non-ui child.
        // We only care about visible UI nodes in a container.
        if (!child->get_visibility() || !child->is_ui_node()) {
            continue;
        }

        auto cast_child = dynamic_cast<NodeUi *>(child.get());
        auto child_min_size = cast_child->get_effective_minimum_size();

        min_size = min_size.max(child_min_size);
    }

    auto tab_button_height = button_container->get_effective_minimum_size().y;
    min_size.y += tab_button_height;

    calculated_minimum_size = min_size;
}

void TabContainer::set_current_tab(int32_t index) {
    current_tab = index;
}

void TabContainer::draw() {
    NodeUi::draw();

    auto vs = VectorServer::get_singleton();

    auto global_pos = get_global_position();

    auto tab_button_height = button_container->get_effective_minimum_size().y;

    vs->draw_style_box(theme_button_panel.value(), global_pos, {size.x, tab_button_height});
    vs->draw_style_box(
        theme_panel.value(), global_pos + Vec2F(0, tab_button_height), size - Vec2F(0, tab_button_height));

    button_container->propagate_draw();
}

void TabContainer::input(InputEvent &event) {
    button_container->propagate_input(event);
}

} // namespace Flint
