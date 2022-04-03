#ifndef FLINT_COMPONENTS_H
#define FLINT_COMPONENTS_H

#include "../../../common/math/vec2.h"
#include "../../../common/math/vec3.h"
#include "../../../common/math/quaternion.h"
#include "../../../rendering/mesh.h"
#include "../../../rendering/material.h"
#include "../../../rendering/mvp_uniform_buffer.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

namespace Flint {
    // PHYSICS
    // -----------------------------
    struct RigidBody {
        Vec3<float> velocity;
        Vec3<float> acceleration;
    };

    struct Transform3D {
        Vec3<float> position;
        Vec3<float> scale;
        Quaternion rotation;
    };

    struct Transform2D {
        Vec2<float> position;
        Vec2<float> scale;
        float rotation = 0;
    };

    struct Gravity {
        Vec3<float> force;
    };
    // -----------------------------

    // RENDER
    // -----------------------------
    struct Sprite2D {
        std::shared_ptr<Mesh2D> mesh;
        std::shared_ptr<Material2D> material;
    };

    struct Sprite3D {

    };

    struct MvpComponent {
        std::shared_ptr<MvpBuffer> mvp_buffer;
    };

    struct TransformGUI {
        Vec2<float> rect_position{0};
        Vec2<float> rect_size{128};
        Vec2<float> rect_scale{1};
        Vec2<float> rect_pivot_offset{0}; // Top-left as the default origin.
        float rect_rotation = 0;
    };

    struct Model {
        std::vector<std::shared_ptr<Mesh3D>> meshes;
        std::vector<std::shared_ptr<Material3D>> materials;

        std::shared_ptr<MvpBuffer> mvp_buffer;
    };
    // -----------------------------
}

#endif //FLINT_COMPONENTS_H
