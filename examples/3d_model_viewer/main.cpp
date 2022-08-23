#include <iostream>
#include <stdexcept>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1920;
const uint32_t WINDOW_HEIGHT = 1080;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Node>();
        auto node_3d = std::make_shared<Node3D>();

        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node->add_child(node_gui);

        auto model0 = std::make_shared<Model>();
        model0->set_mesh(ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        auto model1 = std::make_shared<Model>();
        model1->set_mesh(ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj"));

        auto sub_viewport_c = std::make_shared<SubViewportContainer>();
        node_gui->add_child(sub_viewport_c);

        auto sub_viewport = std::make_shared<SubViewport>();

        // Inspector.
        // ------------------------------------------
        auto inspector_panel = std::make_shared<Panel>();
        inspector_panel->set_title("Inspector");
        inspector_panel->set_size({400, 400});

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 48});
        margin_container->set_size({400, 400 - 48});
        inspector_panel->add_child(margin_container);

        auto vbox_container = std::make_shared<BoxContainer>();
        vbox_container->make_vertical();
        margin_container->add_child(vbox_container);

        auto hbox_container = std::make_shared<BoxContainer>();
        vbox_container->add_child(hbox_container);

        auto file_dialog = std::make_shared<FileDialog>();
        vbox_container->add_child(file_dialog);

        auto line_edit = std::make_shared<LineEdit>();
        hbox_container->add_child(line_edit);

        auto button = std::make_shared<Button>();
        button->set_text("Open");
        // Callback to clean up staging resources.
        auto callback = [file_dialog] {
            Logger::verbose("Button pressed");
            file_dialog->show();
        };
        button->connect_signal("pressed", callback);
        hbox_container->add_child(button);

        auto progress_bar = std::make_shared<ProgressBar>();
        progress_bar->set_size({256, 24});
        vbox_container->add_child(progress_bar);

        // Translation values.
        // ----------------------------------------------------
        auto translation_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>("Translation");
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->sizing_flag = ContainerSizingFlag::Expand;
            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_y->sizing_flag = ContainerSizingFlag::Expand;
            auto spin_box_z = std::make_shared<SpinBox>();
            spin_box_z->sizing_flag = ContainerSizingFlag::Expand;

            auto value_container = std::make_shared<BoxContainer>();
            value_container->sizing_flag = ContainerSizingFlag::Expand;
            value_container->make_vertical();
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
        auto rotation_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>("Rotation");
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->sizing_flag = ContainerSizingFlag::Expand;
            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_y->sizing_flag = ContainerSizingFlag::Expand;
            auto spin_box_z = std::make_shared<SpinBox>();
            spin_box_z->sizing_flag = ContainerSizingFlag::Expand;

            auto value_container = std::make_shared<BoxContainer>();
            value_container->sizing_flag = ContainerSizingFlag::Expand;
            value_container->make_vertical();
            value_container->add_child(spin_box_x);
            value_container->add_child(spin_box_y);
            value_container->add_child(spin_box_z);

            rotation_container->add_child(label);
            rotation_container->add_child(value_container);
        }
        vbox_container->add_child(rotation_container);
        // ----------------------------------------------------

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);
        node->add_child(vector_layer);

        node_gui->add_child(inspector_panel);
        inspector_panel->set_anchor_flag(AnchorFlag::TopRight);

        sub_viewport_c->add_child(sub_viewport);
        sub_viewport_c->set_viewport(sub_viewport);
        sub_viewport_c->set_size({512, 512});
        sub_viewport_c->set_anchor_flag(AnchorFlag::Center);
        sub_viewport->add_child(std::make_shared<Skybox>());
        sub_viewport->add_child(node_3d);
        node_3d->add_child(model1);

        app.tree->get_root()->add_child(node);
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
