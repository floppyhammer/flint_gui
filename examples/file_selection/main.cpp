#include <iostream>
#include <stdexcept>
#include <random>

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
        auto node = std::make_shared<Node>();

        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node->add_child(node_gui);

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);

        auto panel = std::make_shared<Panel>();
        panel->enable_title_bar(false);
        panel->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node_gui->add_child(panel);

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 0});
        margin_container->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        panel->add_child(margin_container);

        auto vbox_container = std::make_shared<BoxContainer>();
        vbox_container->make_vertical();
        margin_container->add_child(vbox_container);

        auto hbox_container = std::make_shared<BoxContainer>();
        vbox_container->add_child(hbox_container);

        auto line_edit = std::make_shared<LineEdit>();
        line_edit->sizing_flag = ContainerSizingFlag::Expand;
        hbox_container->add_child(line_edit);

        auto file_dialog = std::make_shared<FileDialog>();
        node->add_child(file_dialog);

        auto select_button = std::make_shared<Button>();
        select_button->set_text("Select file");
        // Callback to clean up staging resources.
        auto callback = [file_dialog, line_edit] {
            Logger::verbose("Button pressed");
            line_edit->set_text(file_dialog->show());
        };
        select_button->connect_signal("on_pressed", callback);
        hbox_container->add_child(select_button);

        auto confirm_button = std::make_shared<Button>();
        confirm_button->set_text("Confirm");
        vbox_container->add_child(confirm_button);

        node->add_child(vector_layer);
        app.tree->get_root()->add_child(node);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
