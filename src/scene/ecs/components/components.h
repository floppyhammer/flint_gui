#ifndef FLINT_COMPONENTS_H
#define FLINT_COMPONENTS_H

#include "../../../common/math/vec2.h"
#include "../../../common/math/vec3.h"
#include "../../../common/math/quaternion.h"
#include "../../../resources/mesh.h"
#include "../../../resources/material.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/viewport.h"
#include "../../../core/input_event.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <queue>

namespace Flint {
    /// NOTE: Must not allocate any on-heap data in components' default constructors as we will be
    /// using component arrays made up of default components.

    // INPUT
    // -----------------------------
    struct InputComponent {
        std::queue<InputEvent> input_queue;
    };
    // -----------------------------

    // PHYSICS
    // -----------------------------
    struct RigidBodyComponent {
        Vec3<float> velocity{0.0f};
        Vec3<float> acceleration{0.0f};
    };

    struct Transform3dComponent {
        Vec3<float> position{0.0f};
        Vec3<float> scale{1.0f};
        Quaternion rotation;
    };

    struct Transform2dComponent {
        Vec2<float> position{0.0f};
        Vec2<float> scale{1.0f};
        Vec2<float> offset{0.0f}; // Center as the origin.
        float rotation = 0;
    };

    struct GravityComponent {
        Vec3<float> force{0.0f};
    };
    // -----------------------------

    // RENDER
    // -----------------------------
    struct Sprite2dComponent {
        std::shared_ptr<Mesh2d> mesh;
        std::shared_ptr<Mesh2dDescSet> desc_set;
        std::shared_ptr<Material2d> material;
        Mesh2dPushConstant push_constant;
    };

    struct ZSort2d {
        float z;
    };

    struct Sprite3dComponent {

    };

    struct TransformGuiComponent {
        Vec2<float> position{0};
        Vec2<float> size{128};
        Vec2<float> scale{1};
        Vec2<float> pivot_offset{0}; // Top-left as the origin.
        float rotation = 0;
    };

    struct ModelComponent {
        std::vector<std::shared_ptr<Mesh3d>> meshes;
        std::vector<std::shared_ptr<Mesh3dDescSet>> desc_sets;
        std::vector<std::shared_ptr<Material3d>> materials;
        Mesh3dPushConstant push_constant;
    };

    /**
     * Entities with this component will be drawn to a sub-viewport.
     */
    struct ViewportInputComponent {
        std::shared_ptr<Viewport> viewport;

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;
    };

    /**
     * Entities with this component will be used to draw a sub-viewport's content.
     */
    struct ViewportOutputComponent {
        std::shared_ptr<Viewport> viewport;
    };

    /**
     * Entities with this component will make some changes once the window size changes.
     */
    struct WindowSizeAdapterComponent {

    };
    // -----------------------------

    // CUSTOM
    // -----------------------------

    // -----------------------------
}

#endif //FLINT_COMPONENTS_H
