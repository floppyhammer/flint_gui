#include "node_3d.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../common/io.h"

#include <chrono>

namespace Flint {
    void Node3D::self_update(double delta) {
        update_uniform_buffer();
    }

    void Node3D::self_draw() {

    }

    void Node3D::createVertexBuffer() {
        if (mesh != nullptr) return;

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
        if (mesh != nullptr) return;

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
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            RS::getSingleton().createBuffer(bufferSize,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniformBuffers[i],
                                            uniformBuffersMemory[i]);
        }
    }

    void Node3D::update_uniform_buffer() {
        // Prepare UBO data.
        UniformBufferObject ubo{};

        // Determined by model transform.
        ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));

        // Determined by camera.
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

        auto viewport = get_viewport();
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
            //RS::getSingleton().copyDataToMemory(&ubo, uniformBuffersMemory[currentImage], sizeof(ubo));
        } else {
            // Do nothing if no viewport is provided.
        }
    }

    void Node3D::createGraphicsPipeline() {
        auto sub_viewport = get_viewport();
        if (sub_viewport == nullptr) return;

        auto vertShaderCode = readFile("../src/shaders/simple_shader.vert.spv");
        auto fragShaderCode = readFile("../src/shaders/simple_shader.frag.spv");

        VkShaderModule vertShaderModule = RS::getSingleton().createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = RS::getSingleton().createShaderModule(fragShaderCode);

        // Specify shader stages.
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main"; // Specifying the entry point name of the shader for this stage.

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        // Set up the graphics pipeline to accept vertex data.
        // -----------------------------------------------------
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        // -----------------------------------------------------

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) sub_viewport->size.x;
        viewport.height = (float) sub_viewport->size.y;
        viewport.minDepth = 0.0f; // The depth range for the viewport.
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = VkExtent2D{static_cast<uint32_t>(sub_viewport->size.x),
                                    static_cast<uint32_t>(sub_viewport->size.y)};

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        // Create pipeline layout.
        if (vkCreatePipelineLayout(RS::getSingleton().device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.pDepthStencilState = &depthStencil;

        // Create pipeline.
        if (vkCreateGraphicsPipelines(RS::getSingleton().device,
                                      VK_NULL_HANDLE,
                                      1,
                                      &pipelineInfo,
                                      nullptr,
                                      &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        // Clean up shader modules.
        vkDestroyShaderModule(RS::getSingleton().device, fragShaderModule, nullptr);
        vkDestroyShaderModule(RS::getSingleton().device, vertShaderModule, nullptr);
    }
}
