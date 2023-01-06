#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto panel = std::make_shared<Panel>();
        panel->enable_title_bar(false);
        panel->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        panel->set_anchor_flag(AnchorFlag::FullRect);
        panel->apply_fullscreen_style();
        app.tree->replace_scene(panel);

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_anchor_flag(AnchorFlag::FullRect);
        vstack_container->set_separation(16);
        panel->add_child(vstack_container);

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_horizontal_alignment(Alignment::Begin);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_horizontal_alignment(Alignment::Center);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_horizontal_alignment(Alignment::End);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_vertical_alignment(Alignment::Begin);
            label->set_minimum_size(Vec2F(0, 64));
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_vertical_alignment(Alignment::Center);
            label->set_minimum_size(Vec2F(0, 64));
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>("Hello");
            label->set_vertical_alignment(Alignment::End);
            label->set_minimum_size(Vec2F(0, 64));
            vstack_container->add_child(label);
        }
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
