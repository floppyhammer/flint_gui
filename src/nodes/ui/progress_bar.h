#ifndef FLINT_PROGRESS_BAR_H
#define FLINT_PROGRESS_BAR_H

#include <functional>
#include <optional>

#include "../../resources/style_box.h"
#include "label.h"
#include "node_ui.h"

namespace Flint {

class ProgressBar : public NodeUi {
public:
    ProgressBar();

    virtual void custom_update(double dt){};

    void update(double dt) override;

    void draw() override;

    void set_position(Vec2F new_position) override;

    void set_size(Vec2F new_size) override;

    Vec2F calc_minimum_size() const override;

    void set_value(float new_value);

    float get_value() const;

    void set_min_value(float new_value);

    float get_min_value() const;

    void set_max_value(float new_value);

    float get_max_value() const;

    void set_step(float new_step);

    void value_changed();

    void connect_signal(const std::string& signal, const std::function<void()>& callback);

protected:
    float value = 50;
    float min_value = 0;
    float max_value = 100;
    float step = 1;
    float ratio;

    std::optional<StyleBox> theme_progress, theme_bg, theme_fg;

    std::shared_ptr<Label> label;

    std::vector<std::function<void()>> on_value_changed;
};

} // namespace Flint

#endif // FLINT_PROGRESS_BAR_H
