#ifndef FLINT_ECS_RENDER_H
#define FLINT_ECS_RENDER_H

#include <optional>

#include "../common/geometry.h"
#include "../common/mat3x3.h"
#include "../common/quat.h"

using namespace Flint::Math;

namespace Flint::Ecs {

/// A 3D camera projection in which distant objects appear smaller than close objects.
struct C_PerspectiveProjection {
    /// The vertical field of view (FOV) in radians.
    ///
    /// Defaults to a value of π/4 radians or 45 degrees.
    float fov;

    /// The aspect ratio (width divided by height) of the viewing frustum.
    ///
    /// Bevy's [`camera_system`](crate::camera::camera_system) automatically
    /// updates this value when the aspect ratio of the associated window changes.
    ///
    /// Defaults to a value of `1.0`.
    float aspect_ratio;

    /// The distance from the camera in world units of the viewing frustum's near plane.
    ///
    /// Objects closer to the camera than this value will not be visible.
    ///
    /// Defaults to a value of `0.1`.
    float near;

    /// The distance from the camera in world units of the viewing frustum's far plane.
    ///
    /// Objects farther from the camera than this value will not be visible.
    ///
    /// Defaults to a value of `1000.0`.
    float far;
};

/// Holds internally computed [`Camera`] values.
struct ComputedCameraValues {
    Pathfinder::Mat4x4<float> projection_matrix;
    //    std::optional<RenderTargetInfo> target_info;
    // position and size of the `Viewport`
    std::optional<Vec2I> old_viewport_size;
};

/// Render viewport configuration for the [`Camera`] component.
///
/// The viewport defines the area on the render target to which the camera renders its image.
/// You can overlay multiple cameras in a single window using viewports to create effects like
/// split screen, minimaps, and character viewers.
struct Viewport {
    /// The physical position to render this viewport to within the [`RenderTarget`] of this [`Camera`].
    /// (0,0) corresponds to the top-left corner
    Vec2I physical_position;
    /// The physical size of the viewport rectangle to render to within the [`RenderTarget`] of this [`Camera`].
    /// The origin of the rectangle is in the top-left corner.
    Vec2I physical_size;
    /// The minimum and maximum depth to render (on a scale from 0.0 to 1.0).
    Pathfinder::Range depth;
};

/// The "target" that a [`Camera`] will render to. For example, this could be a [`Window`](bevy_window::Window)
/// swapchain or an [`Image`].
struct RenderTarget {
    /// Window to which the camera's view is rendered.
    std::optional<uint32_t> window;
    /// Image to which the camera's view is rendered.
    std::optional<uint32_t> image;
};

/// The defining component for camera entities, storing information about how and what to render
/// through this camera.
///
/// The [`Camera`] component is added to an entity to define the properties of the viewpoint from
/// which rendering occurs. It defines the position of the view to render, the projection method
/// to transform the 3D objects into a 2D image, as well as the render target into which that image
/// is produced.
///
/// Adding a camera is typically done by adding a bundle, either the `Camera2dBundle` or the
/// `Camera3dBundle`.
struct C_Camera {
    /// If set, this camera will render to the given [`Viewport`] rectangle within the configured [`RenderTarget`].
    std::optional<Viewport> viewport;
    /// Cameras with a higher order are rendered later, and thus on top of lower order cameras.
    int32_t order;
    /// If this is set to `true`, this camera will be rendered to its specified [`RenderTarget`]. If `false`, this
    /// camera will not be rendered.
    bool is_active;
    /// Computed values for this camera, such as the projection matrix and the render target size.
    ComputedCameraValues computed;
    /// The "target" that this camera will render to.
    RenderTarget target;
    /// If this is set to `true`, the camera will use an intermediate "high dynamic range" render texture.
    /// Warning: we are still working on this feature. If MSAA is enabled, there will be artifacts in
    /// some cases. When rendering with WebGL, this will crash if MSAA is enabled.
    /// See <https://github.com/bevyengine/bevy/pull/3425> for details.
    // TODO: resolve the issues mentioned in the doc comment above, then remove the warning.
    bool hdr;
    // todo: reflect this when #6042 lands
    /// The [`CameraOutputMode`] for this camera.
    //    CameraOutputMode output_mode;
    /// If this is enabled, a previous camera exists that shares this camera's render target, and this camera has MSAA
    /// enabled, then the previous camera's outputs will be written to the intermediate multi-sampled render target
    /// textures for this camera. This enables cameras with MSAA enabled to "write their results on top" of previous
    /// camera results, and include them as a part of their render results. This is enabled by default to ensure cameras
    /// with MSAA enabled layer their results in the same way as cameras without MSAA enabled by default.
    bool msaa_writeback;
};

struct C_ExtractedCamera {
    //    pub target: Option<NormalizedRenderTarget>,
    std::optional<Vec2I> physical_viewport_size;
    std::optional<Vec2I> physical_target_size;
    std::optional<Viewport> viewport;
    std::string render_graph;
    int32_t order;
    size_t sorted_camera_index_for_target;
};

struct C_Camera2d {
    ColorU clear_color;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_RENDER_H
