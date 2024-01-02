#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto panel = std::make_shared<Panel>();
        panel->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        panel->set_anchor_flag(AnchorFlag::FullRect);
        app.get_tree()->replace_scene(panel);
        {
            StyleBox new_theme;
            new_theme.bg_color = ColorU(27, 27, 27, 255);
            new_theme.corner_radius = 0;
            panel->set_theme_panel(new_theme);
        }

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 0});
        margin_container->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        margin_container->set_margin_all(32);
        margin_container->set_anchor_flag(AnchorFlag::FullRect);
        panel->add_child(margin_container);

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_separation(16);
        margin_container->add_child(vstack_container);

        auto label = std::make_shared<Label>();
        label->set_text("File path:");
        vstack_container->add_child(label);

        auto hstack_container = std::make_shared<HStackContainer>();
        vstack_container->add_child(hstack_container);

        auto text_edit = std::make_shared<TextEdit>();
        text_edit->container_sizing.expand_h = true;
        text_edit->container_sizing.flag_h = ContainerSizingFlag::Fill;
        hstack_container->add_child(text_edit);

        auto file_dialog = std::make_shared<FileDialog>();
        panel->add_child(file_dialog);

        auto select_button = std::make_shared<Button>();
        select_button->set_text("Select");
        // Callback to clean up staging resources.
        auto callback = [file_dialog, text_edit] { text_edit->set_text(file_dialog->show()); };
        select_button->connect_signal("pressed", callback);
        hstack_container->add_child(select_button);

        auto confirm_button = std::make_shared<Button>();
        confirm_button->set_text("Confirm");
        confirm_button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
        vstack_container->add_child(confirm_button);
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
