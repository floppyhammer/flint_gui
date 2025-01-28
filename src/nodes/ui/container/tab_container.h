#ifndef FLINT_TAB_CONTAINER_H
#define FLINT_TAB_CONTAINER_H

#include <optional>

#include "../button.h"
#include "container.h"

namespace Flint {

class TabContainer : public Container {
public:
    TabContainer();

    void update(double dt) override;

    void adjust_layout() override;

    void calc_minimum_size() override;

    void set_current_tab(int32_t index);

    void set_tab_title(uint32_t tab_idx, const std::string& title);

    void set_tab_disabled(bool disabled);

    void draw() override;

    void add_child(const std::shared_ptr<Node> &new_child) override;

protected:
    void add_tab_button();

    std::optional<uint32_t> current_tab;

    std::shared_ptr<HBoxContainer> button_container;

    ButtonGroup tab_button_group;

    std::vector<std::shared_ptr<Button>> tab_buttons;

    std::optional<StyleBox> theme_button_panel;
};

} // namespace Flint

#endif // FLINT_TAB_CONTAINER_H
