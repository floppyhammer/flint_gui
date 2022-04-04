#include "app.h"

#include "rendering/device.h"
#include "rendering/swap_chain.h"
#include "rendering/rendering_server.h"
#include "rendering/texture.h"
#include "core/engine.h"
#include "core/input_event.h"

#include "scene_manager/node/gui/sub_viewport_container.h"
#include "scene_manager/node/3d/model.h"
#include "scene_manager/node/gui/texture_rect.h"
#include "scene_manager/node/sub_viewport.h"

#include "scene_manager/ecs/coordinator.h"
#include "scene_manager/ecs/components/components.h"
#include "scene_manager/ecs/systems/physics_system.h"
#include "scene_manager/ecs/systems/sprite_render_system.h"
#include "scene_manager/ecs/systems/model_render_system.h"

#include <cstdint>
#include <memory>
#include <functional>
#include <random>

void App::run() {
    Flint::Logger::set_level(Flint::Logger::VERBOSE);

    // Initialization.
    // ---------------------------------------------------
    // 1. Initialize hardware.
    auto device = Device::getSingleton();

    // 2. Initialize rendering server.
    auto rs = RS::getSingleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();
    // ---------------------------------------------------

    // Build scene_manager tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Flint::Node>();
        auto node_3d = std::make_shared<Flint::Node3D>();
        auto mesh_instance_0 = std::make_shared<Flint::MeshInstance3D>();
        auto mesh_instance_1 = std::make_shared<Flint::MeshInstance3D>();
        auto sub_viewport_c = std::make_shared<Flint::SubViewportContainer>();
        auto sub_viewport = std::make_shared<Flint::SubViewport>();
        auto texture_rect = std::make_shared<Flint::TextureRect>();
        texture_rect->set_texture(Texture::from_file("../res/texture.jpg"));

        node->add_child(texture_rect);
        node->add_child(mesh_instance_0);
        node->add_child(sub_viewport_c);
        sub_viewport_c->add_child(sub_viewport);
        sub_viewport_c->set_viewport(sub_viewport);
        sub_viewport->add_child(node_3d);
        node_3d->add_child(mesh_instance_1);
        texture_rect->set_rect_scale(0.3, 0.3);
        texture_rect->set_rect_position(400, 0);
        mesh_instance_0->position.x = 1;
        mesh_instance_1->position.x = -1;
        //mesh_instance_1->scale.x = mesh_instance_1->scale.y = mesh_instance_1->scale.z = 0.02;
        tree.set_root(node);
    }

    {
        // ECS test.
        // ----------------------------------------------------------
        auto coordinator = Flint::Coordinator::get_singleton();

        // Register components.
        coordinator.register_component<Flint::Gravity>();
        coordinator.register_component<Flint::RigidBody>();
        coordinator.register_component<Flint::Transform2d>();
        coordinator.register_component<Flint::Transform3d>();
        coordinator.register_component<Flint::TransformGui>();
        coordinator.register_component<Flint::Sprite2d>();
        coordinator.register_component<Flint::Sprite3d>();
        coordinator.register_component<Flint::MvpComponent>();

        // Register systems.
        physics_system = coordinator.register_system<Flint::Physics2dSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator.get_component_type<Flint::Gravity>());
            signature.set(coordinator.get_component_type<Flint::RigidBody>());
            signature.set(coordinator.get_component_type<Flint::Transform2d>());
            coordinator.set_system_signature<Flint::Physics2dSystem>(signature);
        }

        sprite_render_system = coordinator.register_system<Flint::Sprite2dRenderSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator.get_component_type<Flint::Sprite2d>());
            signature.set(coordinator.get_component_type<Flint::Transform2d>());
            signature.set(coordinator.get_component_type<Flint::MvpComponent>());
            coordinator.set_system_signature<Flint::Sprite2dRenderSystem>(signature);
        }

//        auto model_render_system = coordinator.register_system<Flint::ModelRenderSystem>();
//        // Set signature.
//        {
//            Flint::Signature signature;
//            signature.set(coordinator.get_component_type<Flint::Model>());
//            signature.set(coordinator.get_component_type<Flint::Transform3D>());
//            coordinator.set_system_signature<Flint::ModelRenderSystem>(signature);
//        }

        // Allocate space for entities.
        entities.resize(1000);

        std::default_random_engine generator;
        std::uniform_real_distribution<float> rand_position(0.0f, 400.0f);
        std::uniform_real_distribution<float> rand_rotation(0.0f, 3.0f);
        std::uniform_real_distribution<float> rand_velocity(-100.0f, 100.0f);
        std::uniform_real_distribution<float> rand_scale(0.2f, 0.2f);
        std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f);

        auto tex = Texture::from_file("../res/texture.jpg");

        // Create entities.
        for (auto &entity: entities) {
            entity = coordinator.create_entity();

            // Render components.
            {
                auto material = std::make_shared<Material2D>();
                material->texture = tex;

                auto mvp_buffer = std::make_shared<Flint::MvpBuffer>();

                auto mesh = Mesh2D::from_default();
                mesh->updateDescriptorSets(material, mvp_buffer->uniform_buffers);

                coordinator.add_component(
                        entity,
                        Flint::Sprite2d{mesh, material});
                coordinator.add_component(
                        entity,
                        Flint::MvpComponent{mvp_buffer});
            }

            // Physics components.
            {
                coordinator.add_component(
                        entity,
                        Flint::Transform2d{
                                Flint::Vec2<float>(rand_position(generator), rand_position(generator)),
                                Flint::Vec2<float>(0.2),
                                Flint::Vec2<float>(0.2),
                                0.0f,
                        });

                coordinator.add_component(
                        entity,
                        Flint::Gravity{Flint::Vec3<float>(0.0f, 0.0f, 0.0f)});

                coordinator.add_component(
                        entity,
                        Flint::RigidBody{
                                Flint::Vec3<float>(rand_velocity(generator), rand_velocity(generator), 0.0f),
                                Flint::Vec3<float>(0.0f, 0.0f, 0.0f),
                        });
            }
        }
        // ----------------------------------------------------------
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
        glfwSetCursorPosCallback(Device::getSingleton().window, cursor_position_callback);

        auto cursor_button_callback = [](GLFWwindow *window, int button, int action, int mods) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseButton;
            input_event.args.mouse_button.button = button;
            input_event.args.mouse_button.pressed = action == GLFW_PRESS;
            Flint::Logger::verbose("Cursor button", "InputEvent");
        };
        glfwSetMouseButtonCallback(Device::getSingleton().window, cursor_button_callback);
    }

    main_loop();

    // Cleanup.
    {
        // Release node resources.
        tree.set_root(nullptr);

        swap_chain.cleanup();

        rs.cleanup();

        device.cleanup();
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

    // Begin render pass.
    // We can only do this once for the main render pass due to the clear operation.
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain::getSingleton().renderPass;
        renderPassInfo.framebuffer = SwapChain::getSingleton().swapChainFramebuffers[imageIndex]; // Set target framebuffer.
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = SwapChain::getSingleton().swapChainExtent;

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
    }

    // End render pass.
    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    // End recording.
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void App::main_loop() {
    while (!glfwWindowShouldClose(Device::getSingleton().window)) {
        glfwPollEvents();
        draw_frame();
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Device::getSingleton().device);
}

void App::draw_frame() {
    // Engine processing.
    Flint::Engine::getSingleton().tick();
    auto delta = Flint::Engine::getSingleton().get_delta();

    // Acquire next image.
    // We should do this before updating scene as we need to modify different buffers according to the current image index.
    uint32_t imageIndex;
    if (!SwapChain::getSingleton().acquireSwapChainImage(imageIndex)) return;

    // Update the scene.
    {
        // Node scene manager.
        tree.update(delta);

        // ECS scene manager.
        physics_system->update(delta);
        sprite_render_system->update();
    }

    // Record draw calls.
    record_commands(SwapChain::getSingleton().commandBuffers, imageIndex);

    // Submit commands for drawing.
    SwapChain::getSingleton().flush(imageIndex);
}
