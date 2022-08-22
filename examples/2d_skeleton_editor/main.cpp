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

    uint32_t NODE_SPRITE_COUNT = 000;
    uint32_t ECS_SPRITE_COUNT = 000;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand_position(0.0f, 400.0f);
    std::uniform_real_distribution<float> rand_rotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> rand_velocity(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rand_scale(0.2f, 0.2f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f);

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

        auto button = std::make_shared<Button>();
        // Callback to clean up staging resources.
        auto callback = [file_dialog] {
            Logger::verbose("Button pressed");
            file_dialog->show();
        };

        button->connect_signal("on_pressed", callback);
        auto button2 = std::make_shared<Button>();
        hbox_container->add_child(button);
        hbox_container->add_child(button2);

        auto progress_bar = std::make_shared<ProgressBar>();
        progress_bar->set_size({256, 24});
        vbox_container->add_child(progress_bar);

        auto line_edit = std::make_shared<LineEdit>();
        vbox_container->add_child(line_edit);

        // Position values.
        // ----------------------------------------------------
        auto position_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>();
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);
            label->set_text("Position");

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->sizing_flag = ContainerSizingFlag::Expand;
            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_y->sizing_flag = ContainerSizingFlag::Expand;

            auto xy_container = std::make_shared<BoxContainer>();
            xy_container->sizing_flag = ContainerSizingFlag::Expand;
            xy_container->make_vertical();
            xy_container->add_child(spin_box_x);
            xy_container->add_child(spin_box_y);

            position_container->add_child(label);
            position_container->add_child(xy_container);
        }

        vbox_container->add_child(position_container);
        // ----------------------------------------------------

        // Rotation values.
        // ----------------------------------------------------
        auto rotation_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>();
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);
            label->set_text("Rotation");

            auto spin_box = std::make_shared<SpinBox>();
            spin_box->sizing_flag = ContainerSizingFlag::Expand;

            rotation_container->add_child(label);
            rotation_container->add_child(spin_box);
        }
        vbox_container->add_child(rotation_container);
        // ----------------------------------------------------

        // Scene panel.
        // ----------------------------------------------------
        auto scene_panel = std::make_shared<Panel>();
        scene_panel->set_position({50, 100});
        scene_panel->set_title("Scene");
        scene_panel->set_size({400, 400});
        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);

        auto item_tree = std::make_shared<Tree>();
        item_tree->set_size({400, 400});
        // ----------------------------------------------------

        for (int i = 0; i < NODE_SPRITE_COUNT; i++) {
            auto rigid_body_2d = std::make_shared<RigidBody2d>();
            rigid_body_2d->position = {400, 0};
            rigid_body_2d->velocity = {rand_velocity(generator), rand_velocity(generator)};
            auto sprite_2d = std::make_shared<Sprite2d>();
            sprite_2d->set_texture(ResourceManager::get_singleton()->load<ImageTexture>("../assets/duck.png"));
            rigid_body_2d->add_child(sprite_2d);
            node->add_child(rigid_body_2d);
        }

        auto skeleton = std::make_shared<Skeleton2d>();
        skeleton->position = {1000, 300};

        //node->add_child(model0);

        node->add_child(skeleton);

        node->add_child(vector_layer);

        auto margin_container2 = std::make_shared<MarginContainer>();
        margin_container2->set_position({0, 48});
        margin_container2->set_size({400, 400});
        margin_container2->add_child(item_tree);
        scene_panel->add_child(margin_container2);

        node_gui->add_child(scene_panel);
        scene_panel->set_anchor_flag(AnchorFlag::TopLeft);

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

    // ECS test.
    {
        // Add some 2D sprites.
//        for (int i = 0; i < ECS_SPRITE_COUNT; i++) {
//            auto mesh = DefaultResource::get_singleton()->new_default_mesh_2d();
//            mesh->surface->get_material()->set_texture(
//                    ResourceManager::get_singleton()->load<ImageTexture>("../assets/duck.png"));
//
//            auto entity = world->spawn();
//            world->add_component(entity, Sprite2dComponent{mesh});
//            world->add_component(entity, ZSort2d{(float) i / (float) ECS_SPRITE_COUNT});
//            world->add_component(entity, Transform2dComponent{
//                    Vec2<float>(0.0f),
//                    Vec2<float>(1.0f),
//                    Vec2<float>(1.0f),
//                    0.0f});
//            world->add_component(entity, GravityComponent{Vec3<float>(0.0f)});
//            world->add_component(entity, RigidBodyComponent{
//                    Vec3<float>(rand_velocity(generator), rand_velocity(generator), 0.0f),
//                    Vec3<float>(0.0f)});
//        }

        // 3D model.
//        {
//            auto mesh = ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj");
//
//            auto entity = coordinator.create_entity();
//            entities.push_back(entity);
//
//            coordinator.add_component(
//                    entity,
//                    ModelComponent{mesh});
//
//            Transform3dComponent transform;
//            transform.position.x = 0.5;
//            coordinator.add_component(
//                    entity,
//                    transform);
//        }

        // Hierarchy test.
//        {

//            auto entity_0 = world->spawn(Sprite2dComponent{mesh};
//            auto entity_0_0 = world->spawn(Sprite2dComponent{mesh};
//            auto entity_0_1 = world->spawn(Sprite2dComponent{mesh};
//            auto entity_0_0_1 = world->spawn(Sprite2dComponent{mesh};
//
//            coordinator.add_component(
//                    entity_0,
//                    HierarchicalRelations{{entity_0_0},
//                                                 {},
//                                                 {},
//                                                 {}});
//            coordinator.add_component(
//                    entity_0_0,
//                    HierarchicalRelations{{entity_0_0_1},
//                                                 {},
//                                                 {entity_0_1},
//                                                 {entity_0}});
//            coordinator.add_component(
//                    entity_0_1,
//                    HierarchicalRelations{{},
//                                                 {entity_0_1},
//                                                 {},
//                                                 {entity_0}});
//            coordinator.add_component(
//                    entity_0_0_1,
//                    HierarchicalRelations{{},
//                                                 {},
//                                                 {},
//                                                 {entity_0_0_1}});
//        }
//
//        hierarchy_system->traverse(entities[0]);
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
