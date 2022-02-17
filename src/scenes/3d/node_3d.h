#ifndef FLINT_NODE_3D_H
#define FLINT_NODE_3D_H

#include "../node.h"
#include "../../common/vec3.h"
#include "../../rendering/rendering_server.h"
#include "../../rendering/mesh.h"

namespace Flint {
    class Node3D : public Node {
    public:
        // Transform.
        // ------------------------------------------
        Vec3<float> position = Vec3<float>(0);

        Vec3<float> rotation = Vec3<float>(0);

        Vec3<float> scale = Vec3<float>(1);
        // ------------------------------------------

    protected:
        void self_update() override;

        void self_draw() override;

        UniformBufferObject mvp[MAX_FRAMES_IN_FLIGHT];

        void update_uniform_buffer();

        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();

        std::shared_ptr<Mesh> mesh;

        /// Vertex buffer.
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        /// Index buffer.
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        /// We have a uniform buffer per swap chain image.
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        void createGraphicsPipeline();

        /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
        VkDescriptorPool descriptorPool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptorSets;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
    };
}

#endif //FLINT_NODE_3D_H
