#ifndef FLINT_RENDER_SERVER_H
#define FLINT_RENDER_SERVER_H

#include "platform.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include <vector>
#include <iostream>
#include <optional>
#include <array>

namespace Flint {
    /// Vertex for Skeleton2D.
    struct SkeletonVertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 uv; // Texture coordinates.
        glm::vec4 bone_indices; // Can be affected by at most 4 bones.
        glm::vec4 bone_weights; // These 4 bones' weights.

        /// Vertex binding description.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SkeletonVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        /// Vertex attribute descriptions.
        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(SkeletonVertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(SkeletonVertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(SkeletonVertex, uv);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(SkeletonVertex, bone_indices);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(SkeletonVertex, bone_weights);

            return attributeDescriptions;
        }
    };

    /// Vertex for 2D and 3D meshes.
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 uv; // Texture coordinates.

        /// For hashmap.
        bool operator==(const Vertex &other) const {
            return pos == other.pos && color == other.color && uv == other.uv;
        }

        /// Vertex binding description.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

            return bindingDescription;
        }

        /// Vertex attribute descriptions.
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, uv);

            return attributeDescriptions;
        }
    };

    /// Vertex for Skybox.
    struct SkyboxVertex {
        glm::vec3 pos;

        /// Vertex binding description.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SkyboxVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

            return bindingDescription;
        }

        /// Vertex attribute descriptions.
        static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = 0;

            return attributeDescriptions;
        }
    };

    /// GPU data containing a vertex and index buffer.
    template<typename T>
    struct VertexGpuResources {
        VertexGpuResources(const std::vector<T> &vertices, const std::vector<uint32_t> &indices) {
            create_vertex_buffer(vertices);
            create_index_buffer(indices);
        }

        ~VertexGpuResources() {
            auto device = Platform::getSingleton()->device;

            // Clean up index buffer.
            vkDestroyBuffer(device, index_buffer, nullptr);
            vkFreeMemory(device, index_buffer_memory, nullptr);

            // Clean up vertex buffer.
            vkDestroyBuffer(device, vertex_buffer, nullptr); // GPU memory
            vkFreeMemory(device, vertex_buffer_memory, nullptr); // CPU memory
        }

        VkBuffer get_vertex_buffer() {
            return vertex_buffer;
        }

        VkBuffer get_index_buffer() {
            return index_buffer;
        }

        uint32_t get_index_count() {
            return index_count;
        }

    private:
        /// Vertex buffer.
        VkBuffer vertex_buffer{};
        VkDeviceMemory vertex_buffer_memory{};

        /// Index buffer.
        VkBuffer index_buffer{};
        VkDeviceMemory index_buffer_memory{};
        uint32_t index_count = 0;

    private:
        void create_vertex_buffer(const std::vector<T> &vertices) {
            VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

            VkBuffer staging_buffer; // In GPU
            VkDeviceMemory staging_buffer_memory; // In CPU

            auto rs = RenderServer::getSingleton();

            // Create the GPU buffer and link it with the CPU memory.
            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             staging_buffer,
                             staging_buffer_memory);

            // Copy data to the CPU memory.
            rs->copyDataToMemory((void *) vertices.data(), staging_buffer_memory,
                                 buffer_size);

            // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             vertex_buffer,
                             vertex_buffer_memory);

            // Copy buffer (GPU).
            rs->copyBuffer(staging_buffer, vertex_buffer, buffer_size);

            // Clean up staging buffer and memory.
            vkDestroyBuffer(Platform::getSingleton()->device, staging_buffer, nullptr);
            vkFreeMemory(Platform::getSingleton()->device, staging_buffer_memory, nullptr);
        }

        void create_index_buffer(const std::vector<uint32_t> &indices) {
            VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

            // Set indices count for surface.
            index_count = indices.size();

            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;

            auto rs = RenderServer::getSingleton();

            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             staging_buffer,
                             staging_buffer_memory);

            rs->copyDataToMemory((void *) indices.data(),
                                 staging_buffer_memory,
                                 buffer_size);

            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             index_buffer,
                             index_buffer_memory);

            // Copy data from staging buffer to index buffer.
            rs->copyBuffer(staging_buffer, index_buffer, buffer_size);

            vkDestroyBuffer(Platform::getSingleton()->device, staging_buffer, nullptr);
            vkFreeMemory(Platform::getSingleton()->device, staging_buffer_memory, nullptr);
        }
    };

    class RenderServer {
    public:
        static RenderServer *getSingleton() {
            static RenderServer singleton;
            return &singleton;
        }

        RenderServer();

        /**
         * Once swap chain extent changes, render pass also changes. We need recreate pipelines due to that.
         * @param renderPass
         * @param swapChainExtent
         */
        void createSwapChainRelatedResources(VkRenderPass renderPass, VkExtent2D swapChainExtent);

        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    public:
        void cleanup();

        /**
         * Create a shader module, but the shader stage is not specified yet.
         * @return Shader module.
         */
        [[nodiscard]] static VkShaderModule createShaderModule(const std::vector<char> &code);

        /**
         * Create a command buffer in the command pool, and start recording.
         * @return Command buffer.
         */
        [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;

        /**
         * End recording, submit the command buffer to a queue, then free the command buffer in the command pool.
         */
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        void transitionImageLayout(VkImage image,
                                   VkFormat format,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout,
                                   uint32_t levelCount = 1,
                                   uint32_t layerCount = 1) const;

        /**
         * Copy data from VkBuffer to VkImage.
         * @param buffer Src buffer.
         * @param image Dst image.
         * @param offset_x Image region offset x.
         * @param offset_y Image region offset y.
         * @param width Image region width.
         * @param height Image region height.
         */
        void copyBufferToImage(VkBuffer buffer, VkImage image,
                               uint32_t offset_x, uint32_t offset_y,
                               uint32_t width, uint32_t height) const;

        static bool hasStencilComponent(VkFormat format);

        /**
         * Copy data from VkBuffer to VkBuffer.
         * @param srcBuffer Src buffer.
         * @param dstBuffer Dst buffer.
         * @param VkDeviceSize Data size in bytes.
         */
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

        void createImage(uint32_t width,
                         uint32_t height,
                         VkFormat format,
                         VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties,
                         VkImage &image,
                         VkDeviceMemory &imageMemory,
                         uint32_t arrayLayers = 1,
                         VkImageCreateFlags flags = VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM) const;

        /**
         * An image view is a reference to a VkImage.
         * Unlike VkImage, it does not need to be allocated on GPU memory,
         * so you create them directly from the Vulkan API.
         * @param image
         * @param format
         * @param aspectFlags If for color or depth attachment.
         * @return
         */
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

        /**
         * Create GPU buffer and CPU buffer memory and bind them.
         * @param size
         * @param usage
         * @param properties
         * @param buffer
         * @param bufferMemory
         */
        void createBuffer(VkDeviceSize size,
                          VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties,
                          VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory) const;

        /**
         * Copy data to device memory.
         * @param src Source data pointer.
         * @param bufferMemory Device memory.
         * @param dataSize Data size in bytes.
         */
        void
        copyDataToMemory(const void *src, VkDeviceMemory bufferMemory, size_t dataSize, size_t memoryOffset = 0) const;

        void createTextureSampler(VkSampler &textureSampler, VkFilter filter) const;

    public:
        // Various pipelines.
        // --------------------------------------------------
        VkDescriptorSetLayout meshDescriptorSetLayout;
        VkPipelineLayout meshPipelineLayout;
        VkPipeline meshGraphicsPipeline;

        VkDescriptorSetLayout blitDescriptorSetLayout;
        VkPipelineLayout blitPipelineLayout;
        VkPipeline blitGraphicsPipeline;

        VkDescriptorSetLayout skeleton2dMeshDescriptorSetLayout;
        VkPipelineLayout skeleton2dMeshPipelineLayout;
        VkPipeline skeleton2dMeshGraphicsPipeline;

        VkDescriptorSetLayout skybox_descriptor_set_layout;
        VkPipelineLayout skybox_pipeline_layout;
        VkPipeline skybox_graphics_pipeline;

        /**
         * Set up shaders, viewport, blend state, etc.
         * @param renderPass Target render pass.
         * @param viewportExtent Target viewport size.
         * @param graphicsPipeline Generated pipeline.
         * @note We only need one pipeline for a specific rendering process despite of the existence of multiple swap chains.
         * @dependency Descriptor set layout, render pass, viewport extent.
         */
        void createMeshPipeline(VkRenderPass renderPass,
                                VkExtent2D viewportExtent,
                                VkPipeline &pipeline);

        void create_skybox_pipeline(VkRenderPass renderPass,
                                    VkExtent2D viewportExtent,
                                    VkPipeline &pipeline);

        /**
         * Draw a single mesh.
         * @param commandBuffer
         * @param meshGraphicsPipeline
         * @param descriptorSet
         * @param vertexBuffers
         * @param indexBuffer
         * @param indexCount
         */
        void draw_mesh(VkCommandBuffer commandBuffer,
                       VkPipeline meshGraphicsPipeline,
                       VkDescriptorSet const &descriptorSet,
                       VkBuffer *vertexBuffers,
                       VkBuffer indexBuffer,
                       uint32_t indexCount) const;

        void draw_mesh_2d(VkCommandBuffer commandBuffer,
                          VkPipeline graphicsPipeline,
                          const VkDescriptorSet &descriptorSet,
                          VkBuffer vertexBuffers[],
                          VkBuffer indexBuffer,
                          uint32_t indexCount) const;

        void blit(VkCommandBuffer commandBuffer,
                  VkPipeline graphicsPipeline,
                  VkDescriptorSet const &descriptorSet) const;

        void draw_skeleton_2d(VkCommandBuffer command_buffer,
                              VkPipeline pipeline,
                              const VkDescriptorSet &descriptor_set,
                              VkBuffer vertex_buffers[],
                              VkBuffer index_buffer,
                              uint32_t index_count) const;

        void draw_skybox(VkCommandBuffer commandBuffer,
                         VkPipeline meshGraphicsPipeline,
                         VkDescriptorSet const &descriptorSet,
                         VkBuffer *vertexBuffers,
                         VkBuffer indexBuffer,
                         uint32_t indexCount) const;
        // --------------------------------------------------

        void cleanupSwapChainRelatedResources() const;

        void createCommandPool();

        VkCommandPool commandPool;

        void createBlitPipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

        void createSkeleton2dMeshPipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

    private:
        /**
         * Create descriptor set layout and pipeline layout for mesh drawing.
         * @dependency None.
         */
        void createMeshLayouts();

        /**
         * Create descriptor set layout and pipeline layout for blit drawing.
         * @dependency None.
         */
        void createBlitLayouts();

        void createSkeleton2dMeshLayouts();

        void create_skybox_layouts();
    };
}

#endif //FLINT_RENDER_SERVER_H
