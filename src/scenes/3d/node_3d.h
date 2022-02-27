#ifndef FLINT_NODE_3D_H
#define FLINT_NODE_3D_H

#include "../node.h"
#include "../../rendering/rendering_server.h"
#include "../../rendering/mesh.h"

#include <glm/glm.hpp>

namespace Flint {
    class Node3D : public Node {
    public:
        ~Node3D();

        // Transform.
        // ------------------------------------------
        glm::vec3 position = glm::vec3(0);

        glm::vec3 rotation = glm::vec3(0);

        glm::vec3 scale = glm::vec3(1);
        // ------------------------------------------

        void notify(Signal signal) override;

    protected:
        void update(double delta) override;

        void draw() override;

        /**
         * Update MVP. Update UBOs simply by memory mapping.
         * @param currentImage Current image, which has different meaning from `current frame`.
         */
        void updateUniformBuffer();

        /**
         * Create buffer for vertex data.
         * @dependency None.
         */
        void createVertexBuffer();

        /**
         * Create buffer for index data.
         * @dependency None.
         */
        void createIndexBuffer();

        /**
         * Create buffer for uniform data.
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

        /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
        VkDescriptorPool descriptorPool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptorSets;

        bool vkResourcesAllocated = false;
    };
}

#endif //FLINT_NODE_3D_H
