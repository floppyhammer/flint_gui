#include "model.h"

#include "../../../common/io.h"
#include "../../../io/obj_importer.h"
#include "../../../render/swap_chain.h"
#include "../sub_viewport.h"

#include <utility>

namespace Flint {
    const std::string MODEL_NAME = "../assets/viking_room/viking_room.obj";

    Model::Model() {
        type = NodeType::Model;

        // Load model.
        load_file(MODEL_NAME);
    }

    void Model::_update(double delta) {
        // Update self.
        Node3D::update(delta);

        // Update children.
        Node::_update(delta);
    }

    void Model::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void Model::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton().meshGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->viewport->meshGraphicsPipeline;
        }

        for (int i = 0; i < meshes.size(); i++) {
            const auto &mesh = meshes[i];
            const auto &desc_set = desc_sets[i];

            VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
            RenderServer::getSingleton().draw_mesh(
                    p_command_buffer,
                    pipeline,
                    desc_set->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    mesh->indexBuffer,
                    mesh->indices_count);
        }
    }

    void Model::load_file(const std::string &filename) {
        ObjImporter::load_file(filename, meshes, desc_sets, materials, mvp_buffer);
    }
}
