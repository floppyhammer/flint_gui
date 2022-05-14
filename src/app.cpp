#include "app.h"

#include "render/platform.h"
#include "render/swap_chain.h"
#include "render/render_server.h"
#include "resources/image_texture.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "core/engine.h"
#include "servers/input_server.h"
#include "servers/vector_server.h"
#include "io/obj_importer.h"
#include "common/io.h"

#include "scene/node/gui/sub_viewport_container.h"
#include "scene/node/3d/model.h"
#include "scene/node/gui/texture_rect.h"
#include "scene/node/gui/label.h"
#include "scene/node/gui/button.h"
#include "scene/node/gui/box_container.h"
#include "scene/node/gui/margin_container.h"
#include "scene/node/gui/panel.h"
#include "scene/node/gui/tree.h"
#include "scene/node/gui/progress_bar.h"
#include "scene/node/sub_viewport.h"
#include "scene/node/2d/sprite_2d.h"
#include "scene/node/2d/rigid_body_2d.h"
#include "scene/ecs/components/components.h"

#include <cstdint>
#include <memory>
#include <functional>
#include <random>

using namespace Flint;

void App::run() {
    Flint::Logger::set_level(Flint::Logger::VERBOSE);

    // Initialization.
    // ---------------------------------------------------
    // 1. Initialize hardware.
    auto platform = Platform::getSingleton();

    // 2. Initialize render server.
    auto render_server = RenderServer::getSingleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();

    // 4. Initialize input server.
    auto &input_server = Flint::InputServer::get_singleton();
    input_server.attach_callbacks(platform.window);

    // 5. Initialize vector server.
    auto &vector_server = Flint::VectorServer::get_singleton();
    std::shared_ptr<Pathfinder::Driver> driver = std::make_shared<Pathfinder::DriverVk>(platform.device,
                                                                                        platform.physicalDevice,
                                                                                        platform.graphicsQueue,
                                                                                        platform.graphicsQueue,
                                                                                        render_server.commandPool);
    vector_server.init(driver, WIDTH, HEIGHT, readFile("../assets/area-lut.png"));
    // ---------------------------------------------------

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
        auto node = std::make_shared<Flint::Node>();
        auto node_3d = std::make_shared<Flint::Node3D>();
        auto model0 = std::make_shared<Flint::Model>();
        model0->set_mesh(ResourceManager::get_singleton().load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        auto model1 = std::make_shared<Flint::Model>();
        model1->set_mesh(ResourceManager::get_singleton().load<Mesh3d>("../assets/viking_room/viking_room.obj"));
        auto sub_viewport_c = std::make_shared<Flint::SubViewportContainer>();
        auto sub_viewport = std::make_shared<Flint::SubViewport>();
        auto label = std::make_shared<Flint::Label>();
        label->set_text("Hello Flint");
        label->set_horizontal_alignment(Flint::Alignment::Center);
        label->set_vertical_alignment(Flint::Alignment::Center);
        label->set_position({400, 0});
        auto progress_bar = std::make_shared<Flint::ProgressBar>();
        progress_bar->set_position({0, 600});
        progress_bar->set_size({256, 24});
        auto button = std::make_shared<Flint::Button>();
        button->set_position({500, 0});
        // Callback to clean up staging resources.
        auto callback = [] {
            Flint::Logger::verbose("Button pressed");
        };
        button->connect_signal("on_pressed", callback);
        auto button2 = std::make_shared<Flint::Button>();
        auto margin_container = std::make_shared<Flint::MarginContainer>();
        margin_container->set_size({400, 400});
        auto h_stack_container = std::make_shared<Flint::BoxContainer>();
        auto v_stack_container = std::make_shared<Flint::BoxContainer>();
        v_stack_container->make_vertical();
        auto panel = std::make_shared<Flint::Panel>();
        panel->set_position({200, 200});
        panel->set_size({400, 400});

        auto scene_panel = std::make_shared<Flint::Panel>();
        scene_panel->set_position({0, 100});
        scene_panel->set_size({400, 400});
        auto vector_layer = std::make_shared<Flint::TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WIDTH, HEIGHT});
        auto texture_vk = static_cast<Pathfinder::TextureVk *>(vector_server.canvas->get_dest_texture().get());
        auto texture = ImageTexture::from_wrapper(texture_vk->get_image_view(),
                                                  texture_vk->get_sampler(),
                                                  texture_vk->get_width(),
                                                  texture_vk->get_height());
        vector_layer->set_texture(texture);
        auto item_tree = std::make_shared<Flint::Tree>();
        item_tree->set_size({400, 400});

        for (int i = 0; i < NODE_SPRITE_COUNT; i++) {
            auto rigid_body_2d = std::make_shared<Flint::RigidBody2d>();
            rigid_body_2d->position = {400, 0};
            rigid_body_2d->velocity = {rand_velocity(generator), rand_velocity(generator)};
            auto sprite_2d = std::make_shared<Flint::Sprite2d>();
            sprite_2d->set_texture(ResourceManager::get_singleton().load<ImageTexture>("../assets/duck.png"));
            rigid_body_2d->add_child(sprite_2d);
            node->add_child(rigid_body_2d);
        }

        node->add_child(model0);
        node->add_child(sub_viewport_c);

        node->add_child(vector_layer);
        h_stack_container->add_child(button);
        h_stack_container->add_child(label);
        h_stack_container->add_child(button2);
        v_stack_container->add_child(h_stack_container);
        v_stack_container->add_child(progress_bar);
        margin_container->add_child(v_stack_container);
        panel->add_child(margin_container);
        node->add_child(panel);
        auto margin_container2 = std::make_shared<Flint::MarginContainer>();
        margin_container2->set_size({400, 400});
        margin_container2->add_child(item_tree);
        scene_panel->add_child(margin_container2);
        node->add_child(scene_panel);

//        auto btn1 = std::make_shared<Button>();
//        btn1->set_text("");
//        node->add_child(btn1);

        sub_viewport_c->add_child(sub_viewport);
        sub_viewport_c->set_viewport(sub_viewport);
        sub_viewport->add_child(node_3d);
        node_3d->add_child(model1);
        tree.set_root(node);
    }

    // ECS test.
    {
        world = std::make_unique<Flint::World>();

        // Add some 2D sprites.
        for (int i = 0; i < ECS_SPRITE_COUNT; i++) {
            auto mesh = DefaultResource::get_singleton().new_default_mesh_2d();
            mesh->surface->get_material()->set_texture(
                    ResourceManager::get_singleton().load<ImageTexture>("../assets/duck.png"));

            auto entity = world->spawn();
            world->add_component(entity, Flint::Sprite2dComponent{mesh});
            world->add_component(entity, Flint::ZSort2d{(float) i / (float) ECS_SPRITE_COUNT});
            world->add_component(entity, Flint::Transform2dComponent{
                    Flint::Vec2<float>(0.0f),
                    Flint::Vec2<float>(1.0f),
                    Flint::Vec2<float>(1.0f),
                    0.0f});
            world->add_component(entity, Flint::GravityComponent{Flint::Vec3<float>(0.0f)});
            world->add_component(entity, Flint::RigidBodyComponent{
                    Flint::Vec3<float>(rand_velocity(generator), rand_velocity(generator), 0.0f),
                    Flint::Vec3<float>(0.0f)});
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

        // Hierarchy test.
//        {

//            auto entity_0 = world->spawn(Flint::Sprite2dComponent{mesh};
//            auto entity_0_0 = world->spawn(Flint::Sprite2dComponent{mesh};
//            auto entity_0_1 = world->spawn(Flint::Sprite2dComponent{mesh};
//            auto entity_0_0_1 = world->spawn(Flint::Sprite2dComponent{mesh};
//
//            coordinator.add_component(
//                    entity_0,
//                    Flint::HierarchicalRelations{{entity_0_0},
//                                                 {},
//                                                 {},
//                                                 {}});
//            coordinator.add_component(
//                    entity_0_0,
//                    Flint::HierarchicalRelations{{entity_0_0_1},
//                                                 {},
//                                                 {entity_0_1},
//                                                 {entity_0}});
//            coordinator.add_component(
//                    entity_0_1,
//                    Flint::HierarchicalRelations{{},
//                                                 {entity_0_1},
//                                                 {},
//                                                 {entity_0}});
//            coordinator.add_component(
//                    entity_0_0_1,
//                    Flint::HierarchicalRelations{{},
//                                                 {},
//                                                 {},
//                                                 {entity_0_0_1}});
//        }
//
//        hierarchy_system->traverse(entities[0]);
    }

    main_loop();

    // Cleanup.
    {
        // Clean up the scene.
        {
            // Node.
            tree.set_root(nullptr);

            // ECS.
            world.reset();
        }

        DefaultResource::get_singleton().cleanup();

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

    auto &vector_server = Flint::VectorServer::get_singleton();

    vector_server.canvas->clear();

    // Record commands from the scene manager.
    {
        tree.draw(commandBuffers[imageIndex]);

        world->draw(commandBuffers[imageIndex]);
    }

    // End render pass.
    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    // FIXME: When nothing is drawn, the dest image layout will not be set to SHADER_READ_ONLY.
    // Do the vector render pass before the main render pass.
    vector_server.canvas->build_and_render();

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
        tree.input(Flint::InputServer::get_singleton().input_queue);

        // Node scene manager.
        tree.update(dt);

        // ECS scene manager.
        world->update(dt);
    }

    // Record draw calls.
    record_commands(SwapChain::getSingleton().commandBuffers, imageIndex);

    Flint::InputServer::get_singleton().clear_queue();

    // Submit commands for drawing.
    SwapChain::getSingleton().flush(imageIndex);
}
