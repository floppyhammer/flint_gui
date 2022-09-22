#ifndef FLINT_COMPONENTS_H
#define FLINT_COMPONENTS_H

#include "../../../common/geometry.h"
#include "../../../render/mvp.h"
#include "../../../render/render_target.h"
#include "../../../resources/default_resource.h"
#include "../../../resources/material.h"
#include "../../../resources/surface.h"
#include "../../../servers/input_server.h"

#define GLFW_INCLUDE_VULKAN

#include <queue>

#include "GLFW/glfw3.h"

using Pathfinder::Vec2;
using Pathfinder::Vec3;

namespace Flint {
/// NOTE: Must not allocate any on-heap data in components' default constructors as we will be
/// using component arrays made up of default components.

// INPUT
// -----------------------------
struct MouseInputComponent {
    std::queue<InputEvent> input_queue;
};

struct KeyInputComponent {
    std::queue<InputEvent> input_queue;
};
// -----------------------------

// PHYSICS
// -----------------------------
struct RigidBodyComponent {
    Vec3<float> velocity{0.0f};
    Vec3<float> force{0.0f};
};

struct Transform3dComponent {
    Vec3<float> position{0.0f};
    Vec3<float> scale{1.0f};
    // Quaternion rotation;
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
    MvpPushConstant push_constant;
};

struct ZSort2d {
    float z;
};

struct Sprite3dComponent {};

struct TransformGuiComponent {
    Vec2<float> position{0};
    Vec2<float> size{128};
    Vec2<float> scale{1};
    Vec2<float> pivot_offset{0}; // Top-left as the origin.
    float rotation = 0;
};

struct ModelComponent {
    std::shared_ptr<Mesh3d> mesh;
    MvpPushConstant push_constant;
};

/**
 * Entities with this component will be drawn to a sub-viewport.
 */
struct ViewportInputComponent {
    std::shared_ptr<RenderTarget> viewport;

    float fov = 45.0;

    float z_near = 0.1;
    float z_far = 10.0;
};

/**
 * Entities with this component will be used to draw a sub-viewport's content.
 */
struct ViewportOutputComponent {
    std::shared_ptr<RenderTarget> viewport;
};

/**
 * Entities with this component will make some changes once the window size changes.
 */
struct WindowSizeAdapterComponent {};
// -----------------------------

// HIERARCHY
// -----------------------------
struct HierarchicalRelations {
    std::optional<Entity> first_child{};
    std::optional<Entity> prev_sibling{};
    std::optional<Entity> next_sibling{};
    std::optional<Entity> parent{};
};

struct VisibilityComponent {
    bool visible = true;
};
// -----------------------------

// CUSTOM
// -----------------------------

// -----------------------------
} // namespace Flint

#endif // FLINT_COMPONENTS_H
