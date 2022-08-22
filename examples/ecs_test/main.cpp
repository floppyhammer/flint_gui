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

    uint32_t NODE_SPRITE_COUNT = 100;
    uint32_t ECS_SPRITE_COUNT = 100;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand_position(0.0f, 400.0f);
    std::uniform_real_distribution<float> rand_rotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> rand_velocity(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rand_scale(0.2f, 0.2f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Node>();

        for (int i = 0; i < NODE_SPRITE_COUNT; i++) {
            auto rigid_body_2d = std::make_shared<RigidBody2d>();
            rigid_body_2d->position = {400, 0};
            rigid_body_2d->velocity = {rand_velocity(generator), rand_velocity(generator)};
            auto sprite_2d = std::make_shared<Sprite2d>();
            sprite_2d->set_texture(ResourceManager::get_singleton()->load<ImageTexture>("../assets/duck.png"));
            rigid_body_2d->add_child(sprite_2d);
            node->add_child(rigid_body_2d);
        }

        app.tree->get_root()->add_child(node);
    }

    // ECS test.
//    {
//        // Add some 2D sprites.
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
//
//        // 3D model.
////        {
////            auto mesh = ResourceManager::get_singleton()->load<Mesh3d>("../assets/viking_room/viking_room.obj");
////
////            auto entity = coordinator.create_entity();
////            entities.push_back(entity);
////
////            coordinator.add_component(
////                    entity,
////                    ModelComponent{mesh});
////
////            Transform3dComponent transform;
////            transform.position.x = 0.5;
////            coordinator.add_component(
////                    entity,
////                    transform);
////        }
//
//        // Hierarchy test.
////        {
////
////            auto entity_0 = world->spawn(Sprite2dComponent{mesh};
////            auto entity_0_0 = world->spawn(Sprite2dComponent{mesh};
////            auto entity_0_1 = world->spawn(Sprite2dComponent{mesh};
////            auto entity_0_0_1 = world->spawn(Sprite2dComponent{mesh};
////
////            coordinator.add_component(
////                    entity_0,
////                    HierarchicalRelations{{entity_0_0},
////                                                 {},
////                                                 {},
////                                                 {}});
////            coordinator.add_component(
////                    entity_0_0,
////                    HierarchicalRelations{{entity_0_0_1},
////                                                 {},
////                                                 {entity_0_1},
////                                                 {entity_0}});
////            coordinator.add_component(
////                    entity_0_1,
////                    HierarchicalRelations{{},
////                                                 {entity_0_1},
////                                                 {},
////                                                 {entity_0}});
////            coordinator.add_component(
////                    entity_0_0_1,
////                    HierarchicalRelations{{},
////                                                 {},
////                                                 {},
////                                                 {entity_0_0_1}});
////        }
//
//        //hierarchy_system->traverse(entities[0]);
//    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
