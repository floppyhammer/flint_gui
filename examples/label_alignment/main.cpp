#include <iostream>
#include <random>

#include "app.h"

using namespace revector;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

class MyNode : public Node {
    void custom_ready() override {
        auto vbox_container = std::make_shared<VBoxContainer>();
        add_child(vbox_container);
        vbox_container->set_anchor_flag(AnchorFlag::Center);
        vbox_container->set_separation(8);

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Begin);
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Center);
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::End);
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Begin);
            label->set_custom_minimum_size({256, 64});
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Center);
            label->set_custom_minimum_size({256, 64});
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::End);
            label->set_custom_minimum_size({256, 64});
            label->set_theme_bg(StyleBox());
            vbox_container->add_child(label);
        }
    }
};

int main() {
    App app({640, 480});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
