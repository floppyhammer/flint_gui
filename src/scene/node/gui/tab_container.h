#ifndef FLINT_TAB_CONTAINER_H
#define FLINT_TAB_CONTAINER_H

#include "button.h"
#include "container.h"

namespace Flint {

class TabContainer : public Container {
public:
    void update(double dt) override;

    void adjust_layout() override;

    Vec2F calculate_minimum_size() const override;

protected:
    int32_t current_tab = -1;

    std::vector<Button> tab_buttons;
};

} // namespace Flint

#endif // FLINT_TAB_CONTAINER_H
