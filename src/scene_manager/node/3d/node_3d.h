#ifndef FLINT_NODE_3D_H
#define FLINT_NODE_3D_H

#include "../node.h"
#include "../../../rendering/rendering_server.h"
#include "../../../rendering/mesh.h"

#include "glm/glm.hpp"

namespace Flint {
    class Node3D : public Node {
    public:
        Node3D();

        ~Node3D();

        // Transform.
        // ------------------------------------------
        glm::vec3 position{0};

        glm::vec3 rotation{0};

        glm::vec3 scale{1};
        // ------------------------------------------

        void _notify(Signal signal) override;

    protected:
        void _update(double delta) override;

        void update(double delta) override;

        /**
         * Update MVP. Update UBOs simply by memory mapping.
         * @param currentImage Current image, which has different meaning from `current frame`.
         */
        void updateUniformBuffer();

        /**
         * Create buffer for vertex data.
         * @dependency None.
         */
        static void createVertexBuffer(std::vector<Vertex> &vertices,
                                       VkBuffer &p_vertex_buffer,
                                       VkDeviceMemory &p_vertex_buffer_memory);

        /**
         * Create buffer for index data.
         * @dependency None.
         */
        static void createIndexBuffer(std::vector<uint32_t> &indices,
                                      VkBuffer &p_index_buffer,
                                      VkDeviceMemory &p_index_buffer_memory);

        /**
         * Create buffer for uniform data.
         * @dependency Swap chain count.
         */
        void createUniformBuffers();

        /// We have a uniform buffer per swap chain image.
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
    };
}

#endif //FLINT_NODE_3D_H
