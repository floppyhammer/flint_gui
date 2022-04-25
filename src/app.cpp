#include "app.h"

#include "render/platform.h"
#include "render/swap_chain.h"
#include "render/render_server.h"
#include "resources/texture.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "core/engine.h"
#include "core/input_event.h"
#include "io/obj_importer.h"

#include "scene/node/gui/sub_viewport_container.h"
#include "scene/node/3d/model.h"
#include "scene/node/gui/texture_rect.h"
#include "scene/node/sub_viewport.h"
#include "scene/node/2d/sprite_2d.h"
#include "scene/node/2d/rigid_body_2d.h"

#include "scene/ecs/coordinator.h"
#include "scene/ecs/components/components.h"
#include "scene/ecs/systems/physics_system.h"
#include "scene/ecs/systems/sprite_render_system.h"
#include "scene/ecs/systems/model_render_system.h"

#include <cstdint>
#include <memory>
#include <functional>
#include <random>

void App::run() {
    Flint::Logger::set_level(Flint::Logger::VERBOSE);

    // Initialization.
    // ---------------------------------------------------
    // 1. Initialize hardware.
    auto platform = Platform::getSingleton();

    // 2. Initialize rendering server.
    auto render_server = RenderServer::getSingleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();
    // ---------------------------------------------------

    uint32_t NODE_SPRITE_COUNT = 1000;
    uint32_t ECS_SPRITE_COUNT = 1000;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand_position(0.0f, 400.0f);
    std::uniform_real_distribution<float> rand_rotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> rand_velocity(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rand_scale(0.2f, 0.2f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Flint::Node>();
        auto node_3d = std::make_shared<Flint::Node3D>();
        //auto mesh_instance_0 = std::make_shared<Flint::Model>();
        //auto mesh_instance_1 = std::make_shared<Flint::Model>();
        //auto sub_viewport_c = std::make_shared<Flint::SubViewportContainer>();
        //auto sub_viewport = std::make_shared<Flint::SubViewport>();

        for (int i = 0; i < NODE_SPRITE_COUNT; i++) {
            auto rigid_body_2d = std::make_shared<Flint::RigidBody2d>();
            rigid_body_2d->position = {400, 0};
            rigid_body_2d->velocity = {rand_velocity(generator), rand_velocity(generator)};
            auto sprite_2d = std::make_shared<Flint::Sprite2d>();
            sprite_2d->set_texture(ResourceManager::get_singleton().load<Texture>("../assets/duck.png"));
            rigid_body_2d->add_child(sprite_2d);
            node->add_child(rigid_body_2d);
        }

//        node->add_child(mesh_instance_0);
//        node->add_child(sub_viewport_c);
//        sub_viewport_c->add_child(sub_viewport);
//        sub_viewport_c->set_viewport(sub_viewport);
//        sub_viewport->add_child(node_3d);
//        node_3d->add_child(mesh_instance_1);
        tree.set_root(node);
    }

    {
        // ECS test.
        // ----------------------------------------------------------
        auto coordinator = Flint::Coordinator::get_singleton();

        // Register components.
        coordinator.register_component<Flint::GravityComponent>();
        coordinator.register_component<Flint::RigidBodyComponent>();
        coordinator.register_component<Flint::Transform2dComponent>();
        coordinator.register_component<Flint::Transform3dComponent>();
        coordinator.register_component<Flint::TransformGuiComponent>();
        coordinator.register_component<Flint::Sprite2dComponent>();
        coordinator.register_component<Flint::Sprite3dComponent>();
        coordinator.register_component<Flint::ModelComponent>();
        coordinator.register_component<Flint::ViewportInputComponent>();
        coordinator.register_component<Flint::ViewportOutputComponent>();
        coordinator.register_component<Flint::ZSort2d>();

        // Register systems.
        physics_system = coordinator.register_system<Flint::Physics2dSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator.get_component_type<Flint::GravityComponent>());
            signature.set(coordinator.get_component_type<Flint::RigidBodyComponent>());
            signature.set(coordinator.get_component_type<Flint::Transform2dComponent>());
            coordinator.set_system_signature<Flint::Physics2dSystem>(signature);
        }

        sprite_render_system = coordinator.register_system<Flint::Sprite2dRenderSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator.get_component_type<Flint::Sprite2dComponent>());
            signature.set(coordinator.get_component_type<Flint::Transform2dComponent>());
            signature.set(coordinator.get_component_type<Flint::ZSort2d>());
            coordinator.set_system_signature<Flint::Sprite2dRenderSystem>(signature);
        }

        model_render_system = coordinator.register_system<Flint::ModelRenderSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator.get_component_type<Flint::ModelComponent>());
            signature.set(coordinator.get_component_type<Flint::Transform3dComponent>());
            coordinator.set_system_signature<Flint::ModelRenderSystem>(signature);
        }

        // Allocate space for entities.
        entities.resize(ECS_SPRITE_COUNT);

        // 2D sprites.
        int z = 0;
        for (auto &entity: entities) {
            entity = coordinator.create_entity();

            // Render components.
            {
                auto material = std::make_shared<Material2d>();
                material->set_texture(ResourceManager::get_singleton().load<Texture>("../assets/duck.png"));

                coordinator.add_component(
                        entity,
                        Flint::Sprite2dComponent{DefaultResource::get_singleton().default_mesh_2d, material});

                coordinator.add_component(
                        entity,
                        Flint::ZSort2d{(float) z++ / (float) entities.size()});
            }

            // Physics components.
            {
                coordinator.add_component(
                        entity,
                        Flint::Transform2dComponent{
                                Flint::Vec2<float>(0.0f),
                                Flint::Vec2<float>(1.0f),
                                Flint::Vec2<float>(1.0f),
                                0.0f,
                        });

                coordinator.add_component(
                        entity,
                        Flint::GravityComponent{Flint::Vec3<float>(0.0f)});

                coordinator.add_component(
                        entity,
                        Flint::RigidBodyComponent{
                                Flint::Vec3<float>(rand_velocity(generator), rand_velocity(generator), 0.0f),
                                Flint::Vec3<float>(0.0f),
                        });
            }
        }

        // 3D model.
//        {
//            auto mesh = ResourceManager::get_singleton().load<Mesh3d>("../assets/viking_room/viking_room.obj");
//
//            auto entity = coordinator.create_entity();
//            entities.push_back(entity);
//
//            coordinator.add_component(
//                    entity,
//                    Flint::ModelComponent{mesh});
//
//            Flint::Transform3dComponent transform;
//            transform.position.x = 0.5;
//            coordinator.add_component(
//                    entity,
//                    transform);
//        }
    }

    // GLFW input callbacks.
    {
        // A lambda function that doesn't capture anything can be implicitly converted to a regular function pointer.
        auto cursor_position_callback = [](GLFWwindow *window, double x_pos, double y_pos) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseMotion;
            input_event.args.mouse_motion.position = {x_pos, y_pos};
            //Flint::Logger::verbose("Cursor movement", "InputEvent");
        };
        glfwSetCursorPosCallback(Platform::getSingleton().window, cursor_position_callback);

        auto cursor_button_callback = [](GLFWwindow *window, int button, int action, int mods) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseButton;
            input_event.args.mouse_button.button = button;
            input_event.args.mouse_button.pressed = action == GLFW_PRESS;
            Flint::Logger::verbose("Cursor button", "InputEvent");
        };
        glfwSetMouseButtonCallback(Platform::getSingleton().window, cursor_button_callback);
    }

    main_loop();

    // Cleanup.
    {
        // Clean up the scene.
        {
            // Node.
            tree.set_root(nullptr);

            // ECS.
            auto coordinator = Flint::Coordinator::get_singleton();
            for (auto &entity: entities) {
                coordinator.destroy_entity(entity);
            }
            entities.resize(0);
        }

        swap_chain.cleanup();

        render_server.cleanup();

        platform.cleanup();
    }
}

void App::record_commands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex) const {
    // Reset current command buffer.
    vkResetCommandBuffer(commandBuffers[imageIndex], 0);

    // Begin recording.
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin render pass. We bind the target framebuffer here.
    // We can only do this once for the main render pass due to the clear operation.
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain::getSingleton().renderPass;
        renderPassInfo.framebuffer = SwapChain::getSingleton().swapChainFramebuffers[imageIndex]; // Set target framebuffer.
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = SwapChain::getSingleton().swapChainExtent; // Has to be larger than the area we're going to draw.

        // Clear color.
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.02f, 0.02f, 0.02f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex],
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record commands from the scene manager.
    {
        tree.draw(commandBuffers[imageIndex]);

        sprite_render_system->draw(commandBuffers[imageIndex]);

        model_render_system->draw(commandBuffers[imageIndex]);
    }

    // End render pass.
    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    // End recording.
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void App::main_loop() {
    while (!glfwWindowShouldClose(Platform::getSingleton().window)) {
        glfwPollEvents();
        draw_frame();
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Platform::getSingleton().device);
}

void App::draw_frame() {
    // Engine processing.
    Flint::Engine::getSingleton().tick();
    auto dt = Flint::Engine::getSingleton().get_delta();

    // Acquire next image.
    // We should do this before updating scene as we need to modify different buffers according to the current image index.
    uint32_t imageIndex;
    if (!SwapChain::getSingleton().acquireSwapChainImage(imageIndex)) return;

    // Update the scene.
    {
        // Node scene manager.
        tree.update(dt);

        // ECS scene manager.
        physics_system->update(dt);
        sprite_render_system->update();
        model_render_system->update();
    }

    // Record draw calls.
    record_commands(SwapChain::getSingleton().commandBuffers, imageIndex);

    // Submit commands for drawing.
    SwapChain::getSingleton().flush(imageIndex);
}
