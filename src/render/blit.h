#ifndef PATHFINDER_DEMO_TEXTURE_RECT_H
#define PATHFINDER_DEMO_TEXTURE_RECT_H

#include <pathfinder/prelude.h>

#include <memory>

namespace revector {

/**
 * Simple class to blit a texture to screen.
 */
class Blit {
public:
    Blit(const std::shared_ptr<Pathfinder::Device> &_device,
         const std::shared_ptr<Pathfinder::Queue> &_queue,
         Pathfinder::TextureFormat target_format);

    void set_texture(const std::shared_ptr<Pathfinder::Texture> &new_texture);

    void draw(const std::shared_ptr<Pathfinder::CommandEncoder> &encoder);

private:
    std::shared_ptr<Pathfinder::Device> device;

    std::shared_ptr<Pathfinder::Queue> queue;

    std::shared_ptr<Pathfinder::Texture> texture;

    std::shared_ptr<Pathfinder::RenderPipeline> pipeline;

    std::shared_ptr<Pathfinder::Buffer> vertex_buffer;

    std::shared_ptr<Pathfinder::DescriptorSet> descriptor_set;

    std::shared_ptr<Pathfinder::Sampler> sampler;
};

} // namespace revector

#endif
