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

        ~Node3D();

        void notify(Signal signal) override;

    protected:
        void update(double delta) override;

        void draw() override;

        void whenSwapChainChanged();

        /**
         * Update MVP. Update UBOs simply by memory mapping.
         * @param currentImage Current image, which has different meaning from `current frame`.
         */
        void updateUniformBuffer();

        /**
         *
         * @dependency None.
         */
        void createVertexBuffer();

        /**
         *
         * @dependency None.
         */
        void createIndexBuffer();

        /**
         *
         * @dependency Swap chain count.
         */
        void createUniformBuffers();

        /// Contains vertices and indices data.
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

        VkDescriptorSetLayout descriptorSetLayout;

        /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
        VkDescriptorPool descriptorPool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptorSets;
    };
}

#endif //FLINT_NODE_3D_H
