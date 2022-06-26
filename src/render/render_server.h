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
    struct SkeletonVertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 uv;
        glm::vec4 bone_indices;
        glm::vec4 bone_weights;

        bool operator==(const SkeletonVertex &other) const {
            return pos == other.pos && color == other.color && uv == other.uv && bone_indices == other.bone_indices
                   && bone_weights == other.bone_weights;
        }

        /// Binding info.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SkeletonVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        /// Attributes info.
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

    /**
     * Shared by 2D and 3D meshes.
     */
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 uv; // Texture coordinates.

        bool operator==(const Vertex &other) const {
            return pos == other.pos && color == other.color && uv == other.uv;
        }

        /// Binding info.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

            return bindingDescription;
        }

        /// Attributes info.
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

    struct SkyboxVertex {
        glm::vec3 pos;

        /// Binding info.
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SkyboxVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

            return bindingDescription;
        }

        /// Attributes info.
        static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = 0;

            return attributeDescriptions;
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
                         uint32_t arrayLayers = 1) const;

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
        void copyDataToMemory(void *src, VkDeviceMemory bufferMemory, size_t dataSize) const;

        void createTextureSampler(VkSampler &textureSampler, VkFilter filter) const;

    public:
        // Texture rect.
        // --------------------------------------------------

        // --------------------------------------------------

        // Mesh instance 3D.
        // --------------------------------------------------
        // These should be shared by all mesh instances.
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

        void createIndexBuffer(std::vector<uint32_t> &indices,
                               VkBuffer &p_index_buffer,
                               VkDeviceMemory &p_index_buffer_memory);

        void createVertexBuffer(std::vector<Vertex> &vertices,
                                VkBuffer &p_vertex_buffer,
                                VkDeviceMemory &p_vertex_buffer_memory);

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

        void createSkeleton2dMeshPipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

        void create_skybox_layouts();
    };
}

#endif //FLINT_RENDER_SERVER_H
