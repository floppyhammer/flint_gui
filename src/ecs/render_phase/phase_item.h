#ifndef FLINT_RENDER_PHASE_ITEM_H
#define FLINT_RENDER_PHASE_ITEM_H

#include <entt/entt.hpp>

namespace Flint::Ecs {

/// An item (entity of the render world) which will be drawn to a texture or the screen,
/// as part of a [`RenderPhase`].
///
/// The data required for rendering an entity is extracted from the main world in the
/// [`ExtractSchedule`](crate::ExtractSchedule).
/// Then it has to be queued up for rendering during the
/// [`RenderSet::Queue`](crate::RenderSet::Queue), by adding a corresponding phase item to
/// a render phase.
/// Afterwards it will be sorted and rendered automatically in the
/// [`RenderSet::PhaseSort`](crate::RenderSet::PhaseSort) and
/// [`RenderSet::Render`](crate::RenderSet::Render), respectively.
class PhaseItem {
    /// The corresponding entity that will be drawn.
    ///
    /// This is used to fetch the render data of the entity, required by the draw function,
    /// from the render world .
    virtual entt::entity entity() {
        return entt::null;
    };

    /// Determines the order in which the items are drawn.
    virtual int sort_key() {
        return 0;
    };

    /// Specifies the [`Draw`] function used to render the item.
    virtual uint32_t draw_function() {
        return 0;
    };

    /// Sorts a slice of phase items into render order. Generally if the same type
    /// implements [`BatchedPhaseItem`], this should use a stable sort like [`slice::sort_by_key`].
    /// In almost all other cases, this should not be altered from the default,
    /// which uses a unstable sort, as this provides the best balance of CPU and GPU
    /// performance.
    ///
    /// Implementers can optionally not sort the list at all. This is generally advisable if and
    /// only if the renderer supports a depth prepass, which is by default not supported by
    /// the rest of Bevy's first party rendering crates. Even then, this may have a negative
    /// impact on GPU-side performance due to overdraw.
    ///
    /// It's advised to always profile for performance changes when changing this implementation.
    virtual void sort(const std::vector<PhaseItem> &items){};
};

/// A collection of all rendering instructions, that will be executed by the GPU, for a
/// single render phase for a single view.
///
/// Each view (camera, or shadow-casting light, etc.) can have one or multiple render phases.
/// They are used to queue entities for rendering.
/// Multiple phases might be required due to different sorting/batching behaviors
/// (e.g. opaque: front to back, transparent: back to front) or because one phase depends on
/// the rendered texture of the previous phase (e.g. for screen-space reflections).
/// All [`PhaseItem`]s are then rendered using a single [`TrackedRenderPass`].
/// The render pass might be reused for multiple phases to reduce GPU overhead.

template <typename T>
class C_RenderPhase {
    std::vector<std::shared_ptr<T>> _items;

public:
    /// Adds a [`PhaseItem`] to this render phase.
    void add(const std::shared_ptr<T> &item) {
        _items.push_back(item);
    }

    /// Sorts all of its [`PhaseItem`]s.
    void sort() {
    }

    /// Renders all of its [`PhaseItem`]s using their corresponding draw functions.
    void render(
        //        render_pass: &mut TrackedRenderPass<'w>,
        //        world: &'w World,
        entt::entity view) {
    }
};

/// This system sorts the [`PhaseItem`]s of all [`RenderPhase`]s of this type.
template <typename T>
void sort_phase_system(entt::registry &registry) {
    auto view = registry.view<C_RenderPhase<T>>();

    for (auto entity : view) {
        auto &phase = view.get<C_RenderPhase<T>>(entity);
        phase.sort();
    }
}

} // namespace Flint::Ecs

#endif // FLINT_RENDER_PHASE_ITEM_H
