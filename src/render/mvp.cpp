#include "mvp.h"

#include "servers/render_server.h"
#include "swap_chain.h"

namespace Flint {

glm::mat4 ModelViewProjection::calculate_mvp() const {
    return proj * view * model;
}

MvpBuffer::MvpBuffer() {
    create_uniform_buffers();
}

MvpBuffer::~MvpBuffer() {
    free_uniform_buffers();
}

void MvpBuffer::create_uniform_buffers() {
//    auto &swapChainImages = SwapChain::get_singleton()->swapChainImages;
//
//    VkDeviceSize bufferSize = sizeof(ModelViewProjection);
//
//    uniform_buffers.resize(swapChainImages.size());
//    uniform_buffers_memory.resize(swapChainImages.size());
//
//    for (size_t i = 0; i < swapChainImages.size(); i++) {
//        RenderServer::get_singleton()->createBuffer(
//            bufferSize,
//            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//            uniform_buffers[i],
//            uniform_buffers_memory[i]);
//    }
}

void MvpBuffer::update_uniform_buffer(ModelViewProjection mvp) {
    if (uniform_buffers_memory.empty()) {
        return;
    }

//    // Copy the MVP data to the current uniform buffer.
//    RenderServer::get_singleton()->copyDataToMemory(
//        &mvp, uniform_buffers_memory[SwapChain::get_singleton()->currentImage], sizeof(mvp));
}

void MvpBuffer::free_uniform_buffers() {
//    auto device = DisplayServer::get_singleton()->get_device();
//    auto swapChainImages = SwapChain::get_singleton()->swapChainImages;
//
//    // Clean up uniform buffers.
//    for (size_t i = 0; i < swapChainImages.size(); i++) {
//        vkDestroyBuffer(device, uniform_buffers[i], nullptr);
//        vkFreeMemory(device, uniform_buffers_memory[i], nullptr);
//    }
}

} // namespace Flint
