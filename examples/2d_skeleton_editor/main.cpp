#include <iostream>
#include <stdexcept>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

int main() {
    App app;

    app.init();

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
        auto model0 = std::make_shared<Model>();
        model0->set_mesh(ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        auto model1 = std::make_shared<Model>();
        model1->set_mesh(ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        auto sub_viewport_c = std::make_shared<SubViewportContainer>();
        auto sub_viewport = std::make_shared<SubViewport>();

        auto progress_bar = std::make_shared<ProgressBar>();
        progress_bar->set_size({256, 24});
        auto button = std::make_shared<Button>();
        // Callback to clean up staging resources.
        auto callback = [] {
            Logger::verbose("Button pressed");
        };
        button->connect_signal("on_pressed", callback);
        auto button2 = std::make_shared<Button>();

        auto hbox_container = std::make_shared<BoxContainer>();
        auto vbox_container = std::make_shared<BoxContainer>();
        vbox_container->make_vertical();

        // Inspector.
        // ------------------------------------------
        auto inspector_panel = std::make_shared<Panel>();
        inspector_panel->set_position({50, 600});
        inspector_panel->set_title("Inspector");
        inspector_panel->set_size({400, 400});

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_size({400, 400});
        margin_container->add_child(vbox_container);
        inspector_panel->add_child(margin_container);

        vbox_container->add_child(hbox_container);
        vbox_container->add_child(progress_bar);

        auto line_edit = std::make_shared<LineEdit>();
        vbox_container->add_child(line_edit);

        // Position.
        auto position_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>();
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);
            label->set_text("Position");

            auto spin_box_x = std::make_shared<SpinBox>();
            spin_box_x->sizing_flag = ContainerSizingFlag::EXPAND;
            auto spin_box_y = std::make_shared<SpinBox>();
            spin_box_y->sizing_flag = ContainerSizingFlag::EXPAND;

            auto xy_container = std::make_shared<BoxContainer>();
            xy_container->sizing_flag = ContainerSizingFlag::EXPAND;
            xy_container->make_vertical();
            xy_container->add_child(spin_box_x);
            xy_container->add_child(spin_box_y);

            position_container->add_child(label);
            position_container->add_child(xy_container);
        }

        vbox_container->add_child(position_container);
        // ----------------------------------------------------

        // Rotation.
        // ----------------------------------------------------
        auto rotation_container = std::make_shared<BoxContainer>();
        {
            auto label = std::make_shared<Label>();
            label->set_horizontal_alignment(Alignment::Center);
            label->set_vertical_alignment(Alignment::Begin);
            label->set_text("Rotation");

            auto spin_box = std::make_shared<SpinBox>();
            spin_box->sizing_flag = ContainerSizingFlag::EXPAND;

            rotation_container->add_child(label);
            rotation_container->add_child(spin_box);
        }
        vbox_container->add_child(rotation_container);
        // ----------------------------------------------------

        auto node_panel = std::make_shared<Panel>();
        node_panel->set_position({50, 100});
        node_panel->set_title("Scene");
        node_panel->set_size({400, 400});
        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WIDTH, HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        auto item_tree = std::make_shared<Tree>();
        item_tree->set_size({400, 400});

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
        node->add_child(sub_viewport_c);
        node->add_child(skeleton);

        node->add_child(vector_layer);
        hbox_container->add_child(button);
        hbox_container->add_child(button2);
        node->add_child(inspector_panel);
        auto margin_container2 = std::make_shared<MarginContainer>();
        margin_container2->set_size({400, 400});
        margin_container2->add_child(item_tree);
        node_panel->add_child(margin_container2);
        node->add_child(node_panel);

        sub_viewport_c->add_child(sub_viewport);
        sub_viewport_c->set_viewport(sub_viewport);
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
