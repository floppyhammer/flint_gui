#include "text3d.h"

#include "../../resources/font_mesh.h"
#include "../world.h"

namespace Flint {

Text3d::Text3d() {
    type = NodeType::Text3d;

    font_mesh = std::make_shared<FontMesh>("../assets/fonts/Arial Unicode MS Font.ttf");
}

void Text3d::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    auto world = get_world();
    if (!world) {
        return;
    }

    auto camera = get_world()->current_camera3d;
    if (!camera) {
        return;
    }

    VkPipeline pipeline = RenderServer::get_singleton()->get_pipeline(render_pass, "mesh3d");
    VkPipelineLayout pipeline_layout = RenderServer::get_singleton()->mesh_pipeline_layout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);
}

void Text3d::update(double dt) {
    update_mvp();
}

} // namespace Flint
