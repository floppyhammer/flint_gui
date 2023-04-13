#ifndef FLINT_RENDER_PHASE_DRAW_H
#define FLINT_RENDER_PHASE_DRAW_H

#include "phase_item.h"

namespace Flint::Ecs {

/// The result of a [`RenderCommand`].
enum class RenderCommandResult {
    Success,
    Failure,
};

class RenderCommand {
    /// Specifies the general ECS data (e.g. resources) required by [`RenderCommand::render`].
    ///
    /// When fetching resources, note that, due to lifetime limitations of the `Deref` trait,
    /// [`SRes::into_inner`] must be called on each [`SRes`] reference in the
    /// [`RenderCommand::render`] method, instead of being automatically dereferenced as is the
    /// case in normal `systems`.
    ///
    /// All parameters have to be read only.
    ///
    /// [`SRes`]: bevy_ecs::system::lifetimeless::SRes
    /// [`SRes::into_inner`]: bevy_ecs::system::lifetimeless::SRes::into_inner
    //    type Param: SystemParam + 'static;
    /// Specifies the ECS data of the view entity required by [`RenderCommand::render`].
    ///
    /// The view entity refers to the camera, or shadow-casting light, etc. from which the phase
    /// item will be rendered from.
    /// All components have to be accessed read only.
    //                 type ViewWorldQuery: ReadOnlyWorldQuery;
    /// Specifies the ECS data of the item entity required by [`RenderCommand::render`].
    ///
    /// The item is the entity that will be rendered for the corresponding view.
    /// All components have to be accessed read only.
    //    type ItemWorldQuery: ReadOnlyWorldQuery;

    /// Renders a [`PhaseItem`] by recording commands (e.g. setting pipelines, binding bind groups,
    /// issuing draw calls, etc.) via the [`TrackedRenderPass`].
    virtual RenderCommandResult render(std::shared_ptr<PhaseItem> item
                                       //        view: ROQueryItem<'w, Self::ViewWorldQuery>,
                                       //        entity: ROQueryItem<'w, Self::ItemWorldQuery>,
                                       //        param: SystemParamItem<'w, '_, Self::Param>,
                                       //        pass: &mut TrackedRenderPass<'w>,
    );
};

} // namespace Flint::Ecs

#endif // FLINT_RENDER_PHASE_DRAW_H
