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
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node_ui = std::make_shared<NodeUi>();
        app.get_tree()->replace_scene(node_ui);

        // Scene panel.
        // ----------------------------------------------------
        auto scene_panel = std::make_shared<Panel>();
        scene_panel->set_position({50, 100});
        scene_panel->set_size({400, 400});
        scene_panel->set_anchor_flag(AnchorFlag::TopLeft);
        node_ui->add_child(scene_panel);

        auto item_tree = std::make_shared<Tree>();
        item_tree->set_size({400, 400});

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_size({400, 400});
        margin_container->add_child(item_tree);
        scene_panel->add_child(margin_container);
        // ----------------------------------------------------

        auto world2d = std::make_shared<World>(true);
        node_ui->add_child(world2d);

        auto camera2d = std::make_shared<Camera2d>(Vec2I(WINDOW_WIDTH, WINDOW_HEIGHT));
        camera2d->position = {0, 0};
        world2d->add_child(camera2d);
        world2d->add_camera2d(camera2d.get());

        auto node_2d = std::make_shared<Node2d>();
        world2d->add_child(node_2d);

        auto skeleton = std::make_shared<Skeleton2d>();
        skeleton->position = {1000, 300};
        node_2d->add_child(skeleton);
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
