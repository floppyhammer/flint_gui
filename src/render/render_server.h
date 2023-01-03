#ifndef FLINT_RENDER_SERVER_H
#define FLINT_RENDER_SERVER_H

#include "mvp.h"
#include "platform.h"

#define GLFW_INCLUDE_VULKAN

#include <array>
#include <iostream>
#include <optional>
#include <vector>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

namespace Flint {

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

    void transitionImageLayout(VkCommandBuffer cmd_buffer,
                               VkImage image,
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
    void copyBufferToImage(VkCommandBuffer cmd_buffer,
                           VkBuffer buffer,
                           VkImage image,
                           uint32_t offset_x,
                           uint32_t offset_y,
                           uint32_t width,
                           uint32_t height) const;

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
    void copyDataToMemory(const void *src, VkDeviceMemory bufferMemory, size_t dataSize, size_t memoryOffset = 0) const;

    void createTextureSampler(VkSampler &textureSampler, VkFilter filter) const;

public:
    // Various pipelines.
    // --------------------------------------------------
    VkDescriptorSetLayout mesh_descriptor_set_layout{};
    VkPipelineLayout mesh_pipeline_layout{};
    VkPipeline mesh_pipeline{};

    VkDescriptorSetLayout blit_descriptor_set_layout{};
    VkPipelineLayout blit_pipeline_layout{};
    VkPipeline blit_pipeline{};

    VkDescriptorSetLayout skeleton2d_mesh_descriptor_set_layout{};
    VkPipelineLayout skeleton2d_mesh_pipeline_layout{};
    VkPipeline skeleton2d_mesh_pipeline{};

    VkDescriptorSetLayout skybox_descriptor_set_layout{};
    VkPipelineLayout skybox_pipeline_layout{};
    VkPipeline skybox_pipeline{};

    /**
     * Set up shaders, viewport, blend state, etc.
     * @param renderPass Target render pass.
     * @param viewportExtent Target viewport size.
     * @param graphicsPipeline Generated pipeline.
     * @note We only need one pipeline for a specific rendering process despite of the existence of multiple swap
     * chains.
     * @dependency Descriptor set layout, render pass, viewport extent.
     */
    void create_mesh_pipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

    void create_skybox_pipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline) const;

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

    void blit(VkCommandBuffer commandBuffer, VkPipeline graphicsPipeline, VkDescriptorSet const &descriptorSet) const;

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

    VkCommandPool commandPool{};

    void create_blit_pipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

    void create_skeleton2d_mesh_pipeline(VkRenderPass renderPass, VkExtent2D viewportExtent, VkPipeline &pipeline);

private:
    /**
     * Create descriptor set layout and pipeline layout for mesh drawing.
     * @dependency None.
     */
    void create_mesh_layouts();

    /**
     * Create descriptor set layout and pipeline layout for blit drawing.
     * @dependency None.
     */
    void create_blit_layouts();

    void create_skeleton2d_mesh_layouts();

    void create_skybox_layouts();
};

} // namespace Flint

#endif // FLINT_RENDER_SERVER_H
