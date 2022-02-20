#include "mesh_instance_3d.h"

#include <utility>

namespace Flint {
    void MeshInstance3D::set_mesh(std::shared_ptr<Mesh> p_mesh) {
        // Clean previous data.
        if (mesh != nullptr) {
            cleanup();
        }

        mesh = std::move(p_mesh);

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
    }

    std::shared_ptr<Mesh> MeshInstance3D::get_mesh() const {
        return mesh;
    }

    void MeshInstance3D::self_draw() {

    }

    void MeshInstance3D::cleanup() {
        auto device = RS::getSingleton().device;

        // Clean up index buffer.
        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

        // Clean up vertex buffer.
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);

        // Clean up uniform buffers.
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
    }
}
