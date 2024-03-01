#ifndef FLINT_TAB_CONTAINER_H
#define FLINT_TAB_CONTAINER_H

#include <optional>

#include "button.h"
#include "container.h"

namespace Flint {

class TabContainer : public Container {
public:
    TabContainer();

    void update(double dt) override;

    void adjust_layout() override;

    Vec2F calc_minimum_size() const override;

    void set_current_tab(int32_t index);

    void draw() override;

    void input(InputEvent& event) override;

protected:
    int32_t current_tab = -1;

    std::shared_ptr<HStackContainer> button_container;

    ButtonGroup tab_button_group;

    std::vector<std::shared_ptr<Button>> tab_buttons;

    std::optional<StyleBox> theme_panel, theme_button_panel;
};

} // namespace Flint

#endif // FLINT_TAB_CONTAINER_H
