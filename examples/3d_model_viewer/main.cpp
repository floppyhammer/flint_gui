#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node_ui = std::make_shared<NodeUi>();
        node_ui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        app.tree->replace_scene(node_ui);

        auto world3d = std::make_shared<World>(false);
        node_ui->add_child(world3d);

        auto camera3d = std::make_shared<Camera3d>(Vec2I(WINDOW_WIDTH, WINDOW_HEIGHT));
        camera3d->position = {5, 5, 5};
        camera3d->look_at({0, 0, 0});
        world3d->add_child(camera3d);
        world3d->add_camera3d(camera3d.get());

        auto node_3d = std::make_shared<Node3d>();
        world3d->add_child(node_3d);

        auto model = std::make_shared<Model>();
        model->set_mesh(ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        node_3d->add_child(model);

        auto skybox = std::make_shared<Skybox>();
        node_3d->add_child(skybox);

        // Inspector.
        // ------------------------------------------
        auto inspector_panel = std::make_shared<Panel>();
        inspector_panel->set_title("Inspector");
        inspector_panel->set_size({400, 400});
        node_ui->add_child(inspector_panel);

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 48});
        margin_container->set_size({400, 400 - 48});
        inspector_panel->add_child(margin_container);

        auto vbox_container = std::make_shared<VStackContainer>();
        margin_container->add_child(vbox_container);

        // Translation values.
        // ----------------------------------------------------
        auto translation_container = std::make_shared<HStackContainer>();
        {
            auto label = std::make_shared<Label>("Translation");
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;
            auto callback1 = [spin_box_x, model] {
                model->position.x = spin_box_x->get_value();
            };
            spin_box_x->connect_signal("value_changed", callback1);

            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;
            auto spin_box_z = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;

            auto value_container = std::make_shared<VStackContainer>();
            value_container->container_sizing.expand_h = true;

            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;
            value_container->add_child(spin_box_x);
            value_container->add_child(spin_box_y);
            value_container->add_child(spin_box_z);

            translation_container->add_child(label);
            translation_container->add_child(value_container);
        }

        vbox_container->add_child(translation_container);
        // ----------------------------------------------------

        // Rotation values.
        // ----------------------------------------------------
        auto rotation_container = std::make_shared<HStackContainer>();
        {
            auto label = std::make_shared<Label>("Rotation");
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;

            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;
            auto spin_box_z = std::make_shared<SpinBox>();
            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;

            auto value_container = std::make_shared<VStackContainer>();
            value_container->container_sizing.expand_h = true;

            spin_box_x->container_sizing.expand_h = true;
            spin_box_x->container_sizing.flag_h = ContainerSizingFlag::Fill;
            value_container->add_child(spin_box_x);
            value_container->add_child(spin_box_y);
            value_container->add_child(spin_box_z);

            rotation_container->add_child(label);
            rotation_container->add_child(value_container);
        }
        vbox_container->add_child(rotation_container);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
