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

    void draw() override;

protected:
    int32_t current_tab = -1;

    std::shared_ptr<HBoxContainer> button_container;

    ButtonGroup tab_button_group;

    std::vector<std::shared_ptr<Button>> tab_buttons;

    std::optional<StyleBox> theme_panel, theme_button_panel;
};

} // namespace Flint

#endif // FLINT_TAB_CONTAINER_H
