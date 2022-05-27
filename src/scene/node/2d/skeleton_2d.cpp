#include "skeleton_2d.h"

#include "../sub_viewport.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/resource_manager.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "poly2tri/poly2tri.h"

namespace Flint {
    void Bone2d::add_child(std::shared_ptr<Bone2d> child) {
        children.push_back(child);
        child->parent = this;
    }

    Transform2 Bone2d::get_transform() {
        Transform2 transform;
        transform.vector = position;

        // Local rotation in Cartesian coordinates.
        float cartesian_rotation = rotation;
//        if (parent) {
//            auto normalized = position / position.length();
//            cartesian_rotation += atan2(normalized.y, normalized.x);
//        }

        transform = transform.rotate(cartesian_rotation);
        return transform;
    }

    Transform2 Bone2d::get_global_transform() {
        if (parent) {
            return parent->get_global_transform() * get_transform();
        } else {
            if (skeleton) {
                return Transform2::from_translation(skeleton->get_global_position()) * get_transform();
            } else {
                return get_transform();
            }
        }
    }

    void Bone2d::draw() {
        VShape vshape_start;

        // Bone starting point.
        vshape_start.shape.add_circle({0, 0}, 5);
        vshape_start.stroke_color = ColorU(200, 200, 200, 255);
        vshape_start.stroke_width = 3;

        Transform2 start_transform = get_global_transform();
        VectorServer::get_singleton()->draw_vshape(vshape_start, start_transform);

        float arrow_head_length = 6;

        // Draw bone to parent connection.
        if (parent) {
            // Draw in global coordinates.
            auto parent_global_transform = parent->get_global_transform();
            auto distance_to_parent = (start_transform.vector - parent_global_transform.vector).length();

            // If the bone is too short, don't draw the bone body.
            if (distance_to_parent > arrow_head_length) {
                VShape vshape;
                vshape.shape.move_to(arrow_head_length, 0);
                vshape.shape.line_to(arrow_head_length * 2, -arrow_head_length);
                vshape.shape.line_to(distance_to_parent, 0);
                vshape.shape.line_to(arrow_head_length * 2, arrow_head_length);
                vshape.shape.close();
                vshape.fill_color = ColorU(200, 200, 200, 255);
                vshape.stroke_color = ColorU(0, 0, 0, 150);
                vshape.stroke_width = 1;

                // Local rotation in Cartesian coordinates.
                float cartesian_rotation = rotation;
                if (parent) {
                    auto normalized = position / position.length();
                    cartesian_rotation += atan2(normalized.y, normalized.x);
                }
                auto rot_transform = Transform2::from_rotation(cartesian_rotation);
                VectorServer::get_singleton()->draw_vshape(vshape, parent_global_transform * rot_transform);
            }
        }

        // Draw terminal bone.
        if (children.empty()) {
            // If the bone is too short, don't draw the bone body.
            if (length > arrow_head_length) {
                VShape vshape;
                vshape.shape.move_to(arrow_head_length, 0);
                vshape.shape.line_to(arrow_head_length * 2, -arrow_head_length);
                vshape.shape.line_to(length, 0);
                vshape.shape.line_to(arrow_head_length * 2, arrow_head_length);
                vshape.shape.close();
                vshape.fill_color = ColorU(200, 200, 200, 255);
                vshape.stroke_color = ColorU(0, 0, 0, 150);
                vshape.stroke_width = 1;

                VectorServer::get_singleton()->draw_vshape(vshape, start_transform);
            }
        } else {
            for (auto &bone: children) {
                bone->draw();
            }
        }
    }

    Skeleton2d::Skeleton2d() {
        type = NodeType::Skeleton2D;

        sprite = std::make_shared<Sprite2d>();
        sprite->set_texture(ResourceManager::get_singleton()->load<ImageTexture>("../assets/skeleton_2d_texture.png"));
        sprite->set_parent(this);

        base_bone = std::make_shared<Bone2d>();
        base_bone->skeleton = this;

        auto neck_bone = std::make_shared<Bone2d>();
        neck_bone->position = {0, -4};
        neck_bone->length = 4;
        base_bone->add_child(neck_bone);

        auto head_bone = std::make_shared<Bone2d>();
        head_bone->position = {0, -20};
        head_bone->length = 20;
        neck_bone->add_child(head_bone);

        auto left_shoulder_bone = std::make_shared<Bone2d>();
        left_shoulder_bone->position = {-50, 10};
        left_shoulder_bone->length = 50;
        base_bone->add_child(left_shoulder_bone);

        auto left_arm_bone = std::make_shared<Bone2d>();
        left_arm_bone->position = {-50, 10};
        left_arm_bone->length = 50;
        left_shoulder_bone->add_child(left_arm_bone);

        auto right_shoulder_bone = std::make_shared<Bone2d>();
        right_shoulder_bone->position = {50, 10};
        right_shoulder_bone->length = 50;
        base_bone->add_child(right_shoulder_bone);

        auto right_arm_bone = std::make_shared<Bone2d>();
        right_arm_bone->position = {50, 10};
        right_arm_bone->length = 50;
        right_shoulder_bone->add_child(right_arm_bone);

        auto left_hip_bone = std::make_shared<Bone2d>();
        left_hip_bone->position = {-40, 100};
        left_hip_bone->length = 50;
        base_bone->add_child(left_hip_bone);

        auto left_leg_bone = std::make_shared<Bone2d>();
        left_leg_bone->position = {0, 60};
        left_leg_bone->length = 50;
        left_hip_bone->add_child(left_leg_bone);

        auto left_foot_bone = std::make_shared<Bone2d>();
        left_foot_bone->position = {0, 60};
        left_foot_bone->length = 50;
        left_leg_bone->add_child(left_foot_bone);

        auto right_hip_bone = std::make_shared<Bone2d>();
        right_hip_bone->position = {40, 100};
        right_hip_bone->length = 50;
        base_bone->add_child(right_hip_bone);

        auto right_leg_bone = std::make_shared<Bone2d>();
        right_leg_bone->position = {0, 60};
        right_leg_bone->length = 50;
        right_hip_bone->add_child(right_leg_bone);

        auto right_foot_bone = std::make_shared<Bone2d>();
        right_foot_bone->position = {0, 60};
        right_foot_bone->length = 50;
        right_leg_bone->add_child(right_foot_bone);

        // Mesh.
        mesh.vertexes = {
                Vec2F(19.5, -58.1),
                Vec2F(19.7, -24.0),
                Vec2F(14.1, -19.4),
                Vec2F(13.8, -13.6),
                Vec2F(17.6, -11.8),
                Vec2F(24.9, -10.1),
                Vec2F(29.4, -9.1),
                Vec2F(33.5, -8.3),
                Vec2F(62.3, -4.2),
                Vec2F(66.4, -3.9),
                Vec2F(70.4, -3.5),
                Vec2F(149.3, 2.0),
                Vec2F(147.3, 26.6),
                Vec2F(70.1, 21.2),
                Vec2F(64.6, 20.8),
                Vec2F(59.3, 20.5),
                Vec2F(35.2, 20.5),
                Vec2F(33.5, 83.9),
                Vec2F(34.3, 91.2),
                Vec2F(35.5, 98.6),
                Vec2F(38.9, 155.6),
                Vec2F(39.8, 163.3),
                Vec2F(40.8, 169.5),
                Vec2F(53.2, 248.1),
                Vec2F(24.5, 253.0),
                Vec2F(12.7, 175.6),
                Vec2F(11.5, 168.7),
                Vec2F(9.9, 160.7),
                Vec2F(1.4, 124.9),
                Vec2F(-8.0, 159.2),
                Vec2F(-9.7, 166.6),
                Vec2F(-11.7, 174.4),
                Vec2F(-25.7, 252.6),
                Vec2F(-53.4, 247.0),
                Vec2F(-40.5, 170.8),
                Vec2F(-39.7, 163.3),
                Vec2F(-38.9, 155.7),
                Vec2F(-33.6, 100.0),
                Vec2F(-33.2, 92.3),
                Vec2F(-32.7, 84.0),
                Vec2F(-34.8, 21.0),
                Vec2F(-62.3, 21.0),
                Vec2F(-67.7, 21.2),
                Vec2F(-73.8, 21.4),
                Vec2F(-147.2, 24.8),
                Vec2F(-148.4, 0.5),
                Vec2F(-75.1, -2.6),
                Vec2F(-69.2, -3.1),
                Vec2F(-63.1, -3.6),
                Vec2F(-36.1, -6.7),
                Vec2F(-30.1, -7.3),
                Vec2F(-24.5, -8.3),
                Vec2F(-16.7, -12.0),
                Vec2F(-13.0, -14.4),
                Vec2F(-13.0, -19.8),
                Vec2F(-19.0, -24.0),
                Vec2F(-18.9, -58.0),
                Vec2F(-0.4, -40.7),
                Vec2F(-0.6, -1.4),
                Vec2F(-27.6, 6.8),
                Vec2F(-68.6, 8.6),
                Vec2F(-116.9, 10.1),
                Vec2F(27.9, 6.6),
                Vec2F(65.5, 8.5),
                Vec2F(116.0, 11.1),
                Vec2F(-14.1, 97.1),
                Vec2F(-25.2, 165.3),
                Vec2F(-34.3, 237.5),
                Vec2F(15.1, 96.6),
                Vec2F(24.5, 166.3),
                Vec2F(31.8, 237.7),
        };

        mesh.polygons.push_back({44, 43, 60, 46, 45});
        mesh.polygons.push_back({47, 60, 46});
        mesh.polygons.push_back({47, 60, 48});
        mesh.polygons.push_back({60, 43, 42});
        mesh.polygons.push_back({60, 41, 42});
        mesh.polygons.push_back({48, 49, 59, 40, 41, 60});
        mesh.polygons.push_back({49, 59, 50});
        mesh.polygons.push_back({50, 59, 51});
        mesh.polygons.push_back({52, 58, 53});
        mesh.polygons.push_back({53, 54, 58});
        mesh.polygons.push_back({59, 58, 52, 51});
        mesh.polygons.push_back({59, 40, 39, 65, 68, 17, 16, 62, 58});
        mesh.polygons.push_back({58, 2, 3});
        mesh.polygons.push_back({58, 4, 3});
        mesh.polygons.push_back({54, 2, 58});
        mesh.polygons.push_back({55, 56, 0, 1, 2, 54});
        mesh.polygons.push_back({58, 4, 5, 62});
        mesh.polygons.push_back({5, 62, 6});
        mesh.polygons.push_back({62, 7, 6});
        mesh.polygons.push_back({62, 16, 15, 63, 8, 7});
        mesh.polygons.push_back({8, 9, 63});
        mesh.polygons.push_back({9, 10, 63});
        mesh.polygons.push_back({15, 14, 63});
        mesh.polygons.push_back({14, 13, 63});
        mesh.polygons.push_back({63, 10, 11, 12, 13});
        mesh.polygons.push_back({68, 17, 18});
        mesh.polygons.push_back({68, 19, 18});
        mesh.polygons.push_back({27, 69, 20, 19, 68, 28});
        mesh.polygons.push_back({65, 28, 68});
        mesh.polygons.push_back({65, 28, 29, 66, 36, 37});
        mesh.polygons.push_back({65, 37, 38});
        mesh.polygons.push_back({65, 39, 38});
        mesh.polygons.push_back({36, 66, 35});
        mesh.polygons.push_back({35, 34, 66});
        mesh.polygons.push_back({66, 29, 30});
        mesh.polygons.push_back({66, 31, 30});
        mesh.polygons.push_back({66, 31, 32, 33, 34});
        mesh.polygons.push_back({27, 69, 26});
        mesh.polygons.push_back({26, 25, 69});
        mesh.polygons.push_back({69, 20, 21});
        mesh.polygons.push_back({22, 21, 69});
        mesh.polygons.push_back({69, 22, 23, 24, 25});
    }

    template<class C>
    void FreeClear(C &cntr) {
        for (typename C::iterator it = cntr.begin();
             it != cntr.end(); ++it) {
            delete *it;
        }
        cntr.clear();
    }

    bool is_equal(float x, float y, float epsilon = 1e-6) {
        if (std::abs(x - y) < epsilon) return true;
        return false;
    }

    void Skeleton2d::update(double delta) {
        sprite->propagate_update(delta);

        uint32_t vertex_count = mesh.vertexes.size();

        std::vector<Vec2F> points = mesh.vertexes;
        std::vector<Vec2F> uvs;
        std::vector<int> bones;
        std::vector<float> weights;

        if (sprite && sprite->get_texture()) {
            // We assume texture hasn't been rotated, scaled, or moved.
            Transform2 tex_transform;

            auto tex_sizei = sprite->get_texture()->get_size();

            uvs.resize(vertex_count);

            // Map UVs to points.
            if (points.size() == uvs.size()) {
                for (int i = 0; i < vertex_count; i++) {
                    uvs[i] = tex_transform * points[i] / Vec2F(tex_sizei.x, tex_sizei.y);
                }
            } else {
                // Should not happen.
            }
        }

        // Final indices.
        std::vector<uint32_t> total_indices;

        // Go through polygons.
        for (auto &polygon: mesh.polygons) {
            uint32_t polygon_vertex_count = polygon.size();

            // Invalid polygon.
            if (polygon_vertex_count < 3) {
                continue;
            }

            // Constrained triangles.
            std::vector<p2t::Triangle *> triangles;
            // Triangle map.
            std::list<p2t::Triangle *> map;
            // Polylines.
            std::vector<std::vector<p2t::Point *>> polylines;
            std::vector<p2t::Point *> polyline;

            // Get vertexes in this polygon.
            std::vector<Vec2F> polygon_vertexes;
            polygon_vertexes.resize(polygon_vertex_count);

            for (int i = 0; i < polygon_vertex_count; i++) {
                // Vertex indices.
                uint32_t idx = polygon[i];
                assert(idx < mesh.vertexes.size());
                polygon_vertexes[i] = mesh.vertexes[idx];

                // Add points to P2T.
                polyline.push_back(new p2t::Point(polygon_vertexes[i].x, polygon_vertexes[i].y));
            }

            polylines.push_back(polyline);

            std::vector<int> new_local_indices;

            // Generate triangles using P2T. We should not generate any new vertexes.
            {
                auto *cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();

                for (auto tri: triangles) {
                    for (int i = 0; i < 3; i++) {
                        auto p = tri->GetPoint(i);

                        bool found_vertex = false;
                        for (int j = 0; j < polygon_vertex_count; j++) {
                            if (is_equal(p->x, polygon_vertexes[j].x) && is_equal(p->y, polygon_vertexes[j].y)) {
                                new_local_indices.push_back(j);
                                found_vertex = true;
                            }
                        }

                        if (!found_vertex) {
                            Logger::error("Couldn't find the same vertex during triangulation. A new vertex has been generated, which is not allowed!",
                                          "Skeleton2D");
                        }
                    }
                }

                delete cdt;

                // Free points.
                for (auto poly: polylines) {
                    FreeClear(poly);
                }
            }

            uint32_t new_index_count = new_local_indices.size();

            // Add triangle indices to total indices.
            uint32_t index_count = total_indices.size();
            total_indices.resize(index_count + new_index_count);

            // Add new indices.
            for (int i = 0; i < new_index_count; i++) {
                // Global index.
                uint32_t global_idx = polygon[new_local_indices[i]];
                total_indices[index_count + i] = global_idx;
            }
        }

        // Draw all triangles with a single draw call.
        if (total_indices.size()) {
//            RenderServer::get_singleton()->canvas_item_add_triangle_array(total_indices, total_vertexes, colors, uvs, bones, weights, texture);
        }
    }

    void Skeleton2d::draw(VkCommandBuffer p_command_buffer) {
        sprite->propagate_draw(p_command_buffer);

        if (base_bone) {
            base_bone->draw();
        }

        Node2d::draw(p_command_buffer);
    }
}
