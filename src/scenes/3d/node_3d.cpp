#include "node_3d.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../common/io.h"

#include <chrono>

namespace Flint {
    Node3D::~Node3D() {
        auto device = RS::getSingleton().device;
        auto swapChainImages = RS::getSingleton().p_swapChainImages;

        // When we destroy the pool, the sets inside are destroyed as well.
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        // Clean up uniform buffers.
        for (size_t i = 0; i < swapChainImages->size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        // Clean up index buffer.
        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

        // Clean up vertex buffer.
        vkDestroyBuffer(device, vertexBuffer, nullptr); // GPU memory
        vkFreeMemory(device, vertexBufferMemory, nullptr); // CPU memory
    }

    void Node3D::update(double delta) {
        Node::update(delta);

        updateUniformBuffer();
    }

    void Node3D::draw() {

        Node::draw();
    }

    void Node3D::createVertexBuffer() {
        if (mesh == nullptr) return;

        VkDeviceSize bufferSize = sizeof(mesh->vertices[0]) * mesh->vertices.size();

        VkBuffer stagingBuffer; // In GPU
        VkDeviceMemory stagingBufferMemory; // In CPU

        // Create the GPU buffer and link it with the CPU memory.
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        stagingBuffer,
                                        stagingBufferMemory);

        // Copy data to the CPU memory.
        RS::getSingleton().copyDataToMemory(mesh->vertices.data(), stagingBufferMemory, bufferSize);

        // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        vertexBuffer,
                                        vertexBufferMemory);

        // Copy buffer (GPU).
        RS::getSingleton().copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        // Clean up staging buffer and memory.
        vkDestroyBuffer(RS::getSingleton().device, stagingBuffer, nullptr);
        vkFreeMemory(RS::getSingleton().device, stagingBufferMemory, nullptr);
    }

    void Node3D::createIndexBuffer() {
        if (mesh == nullptr) return;

        VkDeviceSize bufferSize = sizeof(mesh->indices[0]) * mesh->indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        stagingBuffer,
                                        stagingBufferMemory);

        RS::getSingleton().copyDataToMemory(mesh->indices.data(),
                                            stagingBufferMemory,
                                            bufferSize);

        RS::getSingleton().createBuffer(bufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        indexBuffer,
                                        indexBufferMemory);

        // Copy data from staging buffer to index buffer.
        RS::getSingleton().copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(RS::getSingleton().device, stagingBuffer, nullptr);
        vkFreeMemory(RS::getSingleton().device, stagingBufferMemory, nullptr);
    }

    void Node3D::createUniformBuffers() {
        auto &swapChainImages = RS::getSingleton().p_swapChainImages;

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapChainImages->size());
        uniformBuffersMemory.resize(swapChainImages->size());

        for (size_t i = 0; i < swapChainImages->size(); i++) {
            RS::getSingleton().createBuffer(bufferSize,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniformBuffers[i],
                                            uniformBuffersMemory[i]);
        }
    }

    void Node3D::updateUniformBuffer() {
        if (uniformBuffersMemory.empty()) return;

        // Prepare UBO data.
        UniformBufferObject ubo{};

        // Determined by model transform.
        ubo.model = glm::rotate(glm::mat4(1.0f), (float) Engine::getSingleton().get_elapsed() * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));

        // Determined by camera.
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

        //auto viewport = get_viewport();
        auto viewport = std::make_shared<SubViewport>();
        viewport->size = Vec2<int>(RS::getSingleton().p_swapChainExtent.width,
                                   RS::getSingleton().p_swapChainExtent.height);

        if (viewport != nullptr) {
            // Set projection matrix. Determined by viewport.
            ubo.proj = glm::perspective(glm::radians(viewport->fov),
                                        (float) viewport->size.x / (float) viewport->size.y,
                                        viewport->z_near,
                                        viewport->z_far);

            // GLM was originally designed for OpenGL,
            // where the Y coordinate of the clip coordinates is inverted.
            ubo.proj[1][1] *= -1;

            // Copy the UBO data to the current uniform buffer.
            RS::getSingleton().copyDataToMemory(&ubo,
                                                uniformBuffersMemory[RS::getSingleton().currentImage],
                                                sizeof(ubo));
        } else {
            // Do nothing if no viewport is provided.
        }
    }

    void Node3D::notify(Signal signal) {
        Node::notify(signal);

        switch (signal) {
            case Signal::SwapChainChanged: {

            }
                break;
            default:
                break;
        }
    }
}
