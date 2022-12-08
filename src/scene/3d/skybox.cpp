#include "skybox.h"

#include "../../common/load_file.h"
#include "../../common/obj_importer.h"
#include "../../render/swap_chain.h"
#include "../../resources/default_resource.h"
#include "../../resources/resource_manager.h"
#include "../world.h"
#include "camera3d.h"

namespace Flint {

Skybox::Skybox() {
    type = NodeType::Skybox;

    material = std::make_shared<MaterialSkybox>();
    material->set_texture(std::make_shared<CubemapTexture>("../assets/skybox.png"));
}

void Skybox::update(double dt) {
    update_mvp();
}

void Skybox::draw(VkCommandBuffer cmd_buffer) {
    auto world = get_world();
    if (!world) {
        return;
    }

    auto camera = get_world()->current_camera3d;
    if (!camera) {
        return;
    }

    VkPipeline pipeline = camera->render_target->skybox_graphics_pipeline;
    VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->skybox_pipeline_layout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    const auto &desc_set = material->get_desc_set();

    auto skybox_gpu_resource = DefaultResource::get_singleton()->get_default_skybox_gpu_resources();

    VkBuffer vertexBuffers[] = {skybox_gpu_resource->get_vertex_buffer()};
    RenderServer::getSingleton()->draw_skybox(cmd_buffer,
                                              pipeline,
                                              desc_set->getDescriptorSet(SwapChain::getSingleton()->currentImage),
                                              vertexBuffers,
                                              skybox_gpu_resource->get_index_buffer(),
                                              skybox_gpu_resource->get_index_count());
}

} // namespace Flint
