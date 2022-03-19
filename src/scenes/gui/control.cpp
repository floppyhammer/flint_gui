#include "control.h"

#include "../sub_viewport.h"
#include "../../rendering/swap_chain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Flint {
    void Control::set_rect_position(float x, float y) {
        rect_position.x = x;
        rect_position.y = y;
    }

    void Control::set_rect_size(float w, float h) {
        rect_size.x = w;
        rect_size.y = h;
    }

    void Control::set_rect_scale(float x, float y) {
        rect_scale.x = x;
        rect_scale.y = y;
    }

    void Control::set_rect_rotation(float r) {
        rect_rotation = r;
    }

    void Control::set_rect_pivot_offset(float x, float y) {
        rect_pivot_offset.x = x;
        rect_pivot_offset.y = y;
    }

    void Control::update(double delta) {
        // Branch to root.
        Node::update(delta);

        update_uniform_buffer();
    }

    void Control::update_uniform_buffer() {
        if (uniform_buffers_memory.empty()) return;

        // Prepare UBO data.
        UniformBufferObject ubo{};
        ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(rect_position.x / rect_size.x * 2.0f,
                                                              rect_position.y / rect_size.y * 2.0f,
                                                              0.0f));
        ubo.model = glm::scale(ubo.model, glm::vec3(rect_scale.x, rect_scale.y, 1.0f));

        Node *viewport_node = get_viewport();

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
        } else { // Default to swap chain image.

        }

        // Copy the UBO data to the current uniform buffer.
        RS::getSingleton().copyDataToMemory(&ubo.model,
                                            uniform_buffers_memory[SwapChain::getSingleton().currentImage],
                                            sizeof(ubo.model));
    }

    void Control::create_vertex_buffer() {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer; // In GPU
        VkDeviceMemory stagingBufferMemory; // In CPU

        // Create the GPU buffer and link it with the CPU memory.
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        stagingBuffer,
                                        stagingBufferMemory);

        // Copy data to the CPU memory.
        RS::getSingleton().copyDataToMemory((void *) vertices.data(), stagingBufferMemory, bufferSize);

        // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        vertex_buffer,
                                        vertex_buffer_memory);

        // Copy buffer (GPU).
        RS::getSingleton().copyBuffer(stagingBuffer, vertex_buffer, bufferSize);

        // Clean up staging buffer and memory.
        vkDestroyBuffer(Device::getSingleton().device, stagingBuffer, nullptr);
        vkFreeMemory(Device::getSingleton().device, stagingBufferMemory, nullptr);
    }

    void Control::create_index_buffer() {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        stagingBuffer,
                                        stagingBufferMemory);

        RS::getSingleton().copyDataToMemory((void *) indices.data(),
                                            stagingBufferMemory,
                                            bufferSize);

        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        index_buffer,
                                        index_buffer_memory);

        // Copy data from staging buffer to index buffer.
        RS::getSingleton().copyBuffer(stagingBuffer, index_buffer, bufferSize);

        vkDestroyBuffer(Device::getSingleton().device, stagingBuffer, nullptr);
        vkFreeMemory(Device::getSingleton().device, stagingBufferMemory, nullptr);
    }

    void Control::create_uniform_buffers() {
        auto &swapChainImages = SwapChain::getSingleton().swapChainImages;

        VkDeviceSize bufferSize = sizeof(glm::mat4);

        uniform_buffers.resize(swapChainImages.size());
        uniform_buffers_memory.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            RS::getSingleton().createBuffer(bufferSize,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniform_buffers[i],
                                            uniform_buffers_memory[i]);
        }
    }
}
