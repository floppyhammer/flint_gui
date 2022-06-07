#ifndef FLINT_SURFACE_H
#define FLINT_SURFACE_H

#include "material.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <string>
#include <utility>
#include <vector>
#include <array>

namespace Flint {
    struct Surface2dPushConstant {
        glm::mat4 model;
    };

    // TODO: We might need to push model, view and projection separately.
    struct Surface3dPushConstant {
        glm::mat4 mvp;
    };

    /// A surface may contain a material, which may be shared by other surfaces.

    template <typename T>
    struct SurfaceGpuResources {
        SurfaceGpuResources(const std::vector<T> &vertices, const std::vector<uint32_t> &indices) {
            create_vertex_buffer(vertices);
            create_index_buffer(indices);
        }

        ~SurfaceGpuResources() {
            auto device = Platform::getSingleton()->device;

            // Clean up index buffer.
            vkDestroyBuffer(device, indexBuffer, nullptr);
            vkFreeMemory(device, indexBufferMemory, nullptr);

            // Clean up vertex buffer.
            vkDestroyBuffer(device, vertexBuffer, nullptr); // GPU memory
            vkFreeMemory(device, vertexBufferMemory, nullptr); // CPU memory
        }

        /// Vertex buffer.
        VkBuffer vertexBuffer{};
        VkDeviceMemory vertexBufferMemory{};

        /// Index buffer.
        VkBuffer indexBuffer{};
        VkDeviceMemory indexBufferMemory{};
        uint32_t indices_count = 0;

    private:
        void create_vertex_buffer(const std::vector<T> &vertices) {
            VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

            VkBuffer stagingBuffer; // In GPU
            VkDeviceMemory stagingBufferMemory; // In CPU

            // Create the GPU buffer and link it with the CPU memory.
            RenderServer::getSingleton()->createBuffer(bufferSize,
                                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                       stagingBuffer,
                                                       stagingBufferMemory);

            // Copy data to the CPU memory.
            RenderServer::getSingleton()->copyDataToMemory((void *) vertices.data(), stagingBufferMemory, bufferSize);

            // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
            RenderServer::getSingleton()->createBuffer(bufferSize,
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       vertexBuffer,
                                                       vertexBufferMemory);

            // Copy buffer (GPU).
            RenderServer::getSingleton()->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

            // Clean up staging buffer and memory.
            vkDestroyBuffer(Platform::getSingleton()->device, stagingBuffer, nullptr);
            vkFreeMemory(Platform::getSingleton()->device, stagingBufferMemory, nullptr);
        }

        void create_index_buffer(const std::vector<uint32_t> &indices) {
            VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

            // Set indices count for surface.
            indices_count = indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            RenderServer::getSingleton()->createBuffer(bufferSize,
                                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                       stagingBuffer,
                                                       stagingBufferMemory);

            RenderServer::getSingleton()->copyDataToMemory((void *) indices.data(),
                                                           stagingBufferMemory,
                                                           bufferSize);

            RenderServer::getSingleton()->createBuffer(bufferSize,
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       indexBuffer,
                                                       indexBufferMemory);

            // Copy data from staging buffer to index buffer.
            RenderServer::getSingleton()->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(Platform::getSingleton()->device, stagingBuffer, nullptr);
            vkFreeMemory(Platform::getSingleton()->device, stagingBufferMemory, nullptr);
        }
    };

    class Surface {
    public:
        Surface() = default;

        std::string name;

        VkBuffer get_vertex_buffer();

        VkBuffer get_index_buffer();

        uint32_t get_index_count();

        void set_gpu_resources(std::shared_ptr<SurfaceGpuResources<Vertex>> p_gpu_resources);

    private:
        std::shared_ptr<SurfaceGpuResources<Vertex>> gpu_resources;
    };

    class Surface2d : public Surface {
    public:
        Surface2d() = default;

        /**
         * Default quad surface.
         */
        static std::shared_ptr<Surface2d> from_default();

        void set_material(const std::shared_ptr<Material2d> &p_material);

        std::shared_ptr<Material2d> get_material() const;

    private:

        std::shared_ptr<Material2d> material;
    };

    class Surface3d : public Surface {
    public:
        Surface3d() = default;

        static std::shared_ptr<Surface3d> from_plane() {
            return std::make_shared<Surface3d>();
        }

        static std::shared_ptr<Surface3d> from_cube() {
            return std::make_shared<Surface3d>();
        }

        static std::shared_ptr<Surface3d> from_sphere() {
            return std::make_shared<Surface3d>();
        }

        void set_material(const std::shared_ptr<Material3d> &p_material);

        std::shared_ptr<Material3d> get_material() const;

    private:
        std::shared_ptr<Material3d> material;
    };
}

#endif //FLINT_SURFACE_H
