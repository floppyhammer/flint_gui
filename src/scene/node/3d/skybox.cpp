#include "skybox.h"

#include "../../../common/io.h"
#include "../../../io/obj_importer.h"
#include "../../../resources/resource_manager.h"
#include "../../../resources/default_resource.h"
#include "../../../render/swap_chain.h"
#include "../sub_viewport.h"

namespace Flint {
    Skybox::Skybox() {
        type = NodeType::Skybox;

        material = std::make_shared<MaterialSkybox>();
        material->set_texture(std::make_shared<CubemapTexture>("../assets/skybox.png"));
    }

    void Skybox::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton()->skybox_graphics_pipeline;
        VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->skybox_pipeline_layout;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->render_target->skybox_graphics_pipeline;
        }

        // Upload the model matrix to the GPU via push constants.
        vkCmdPushConstants(p_command_buffer, pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(MvpPushConstant), &push_constant);

        const auto &desc_set = material->get_desc_set();

        auto skybox_gpu_resource = DefaultResource::get_singleton()->get_default_skybox_gpu_resources();

        VkBuffer vertexBuffers[] = {skybox_gpu_resource->get_vertex_buffer()};
        RenderServer::getSingleton()->draw_skybox(
                p_command_buffer,
                pipeline,
                desc_set->getDescriptorSet(SwapChain::getSingleton()->currentImage),
                vertexBuffers,
                skybox_gpu_resource->get_index_buffer(),
                skybox_gpu_resource->get_index_count());
    }
}
