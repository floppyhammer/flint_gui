#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Node>();

        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node->add_child(node_gui);

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);

        auto image_texture = ResourceManager::get_singleton()->load<ImageTexture>("../assets/duck.png");
        auto vector_texture = ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Button.svg");

        for (int i = 0; i < 7; i++) {
            auto texture_rect_svg = std::make_shared<TextureRect>();
            texture_rect_svg->set_position({400.0f, i * 100.0f});
            texture_rect_svg->set_size({200, 100});
            texture_rect_svg->set_texture(vector_texture);
            texture_rect_svg->set_debug_mode(true);
            node_gui->add_child(texture_rect_svg);

            auto texture_rect_image = std::make_shared<TextureRect>();
            texture_rect_image->set_texture(image_texture);
            texture_rect_image->set_position({0.0f, i * 100.0f});
            texture_rect_image->set_debug_mode(true);
            texture_rect_image->set_size({200, 100});
            node_gui->add_child(texture_rect_image);

            if (i == 0) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::Keep);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::Keep);
            }
            if (i == 1) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::KeepCentered);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::KeepCentered);
            }
            if (i == 2) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::Scale);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::Scale);
            }
            if (i == 4) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::KeepAspect);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::KeepAspect);
            }
            if (i == 5) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::KeepAspectCentered);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::KeepAspectCentered);
            }
            if (i == 6) {
                texture_rect_svg->set_stretch_mode(TextureRect::StretchMode::KeepCovered);
                texture_rect_image->set_stretch_mode(TextureRect::StretchMode::KeepCovered);
            }
        }

        node->add_child(vector_layer);
        app.tree->get_root()->add_child(node);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
