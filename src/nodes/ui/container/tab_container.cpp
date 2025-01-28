#include "tab_container.h"

#include "../../../resources/default_resource.h"

namespace Flint {

TabContainer::TabContainer() {
    type = NodeType::TabContainer;

    auto default_theme = DefaultResource::get_singleton()->get_default_theme();

    theme_bg_ = std::make_optional(default_theme->panel.styles["background"]);
    theme_bg_.value().corner_radius = 0;
    theme_bg_.value().border_width = 0;

    theme_button_panel = std::make_optional<StyleBox>();
    theme_button_panel.value().corner_radius = 0;
    theme_button_panel.value().border_width = 0;
    theme_button_panel.value().bg_color = ColorU(20, 20, 20);

    button_container = std::make_shared<HBoxContainer>();
    add_embedded_child(button_container);
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
    tab_buttons[index]->press();
    tab_button_group.pressed_button = tab_buttons[index];
}

void TabContainer::draw() {
    Container::draw();

    auto vs = VectorServer::get_singleton();

    auto global_pos = get_global_position();

    auto tab_button_height = button_container->get_effective_minimum_size().y;

    vs->draw_style_box(theme_button_panel.value(), global_pos, {size.x, tab_button_height});
}

void TabContainer::add_child(const std::shared_ptr<Node> &new_child) {
    Node::add_child(new_child);

    add_tab_button();

    if (children.size() == 1) {
        set_current_tab(0);
    }
}

void TabContainer::add_tab_button() {
    auto button = std::make_shared<Button>();
    button_container->add_child(button);

    uint32_t button_idx = tab_buttons.size();
    button->set_text("Tab " + std::to_string(button_idx));

    tab_buttons.push_back(button);

    tab_button_group.add_button(button);

    auto callback = [this, button_idx] { current_tab = button_idx; };
    button->connect_signal("pressed", callback);
    button->set_toggle_mode(true);

    button->theme_normal.border_width = 0;
    button->theme_normal.corner_radius = 0;
    button->theme_normal.bg_color = ColorU(20, 20, 20);

    button->theme_hovered.border_width = 0;
    button->theme_hovered.corner_radius = 0;
    button->theme_hovered.bg_color = ColorU(40, 40, 40);

    button->theme_pressed.border_width = 0;
    button->theme_pressed.corner_radius = 0;
    button->theme_pressed.bg_color = ColorU(32, 32, 32);
}

void TabContainer::set_tab_title(uint32_t tab_idx, const std::string &title) {
    if (tab_idx >= tab_buttons.size()) {
        return;
    }
    tab_buttons[tab_idx]->set_text(title);
}

void TabContainer::set_tab_disabled(bool disabled) {
    for (auto& b : tab_buttons) {
        b->set_disabled(disabled);
    }
}

} // namespace Flint
