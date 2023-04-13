#ifndef FLINT_GRAPH_SLOT_H
#define FLINT_GRAPH_SLOT_H

#include <entt/entt.hpp>
#include <optional>
#include <string>
#include <vector>

#include "../../servers/display_server.h"

namespace Flint::Ecs {

/// A [`SlotLabel`] is used to reference a slot by either its name or index
/// inside the [`RenderGraph`](super::RenderGraph).
struct SlotLabel {
    enum class Type {
        Index,
        Name,
    } type;

    size_t index;
    std::string name;

    static SlotLabel from_index(size_t index_) {
        return {Type::Index, index_, ""};
    }

    static SlotLabel from_name(std::string name_) {
        return {Type::Name, {}, std::move(name_)};
    }
};

/// Describes the render resources created (output) or used (input) by
/// the render [`Nodes`](super::Node).
///
/// This should not be confused with [`SlotValue`], which actually contains the passed data.
enum class SlotType {
    /// A GPU-accessible [`Buffer`].
    Buffer,
    /// A [`TextureView`] describes a texture used in a pipeline.
    TextureView,
    /// A texture [`Sampler`] defines how a pipeline will sample from a [`TextureView`].
    Sampler,
    /// An entity from the ECS.
    Entity,
};

/// The internal representation of a slot, which specifies its [`SlotType`] and name.
struct SlotInfo {
    std::string name;
    SlotType slot_type;
};

/// A collection of input or output [`SlotInfos`](SlotInfo) for
/// a [`NodeState`](super::NodeState).
struct SlotInfos {
    std::vector<SlotInfo> slots;

    /// Retrieves the [`SlotInfo`] for the provided label.
    std::optional<SlotInfo> get_slot(const SlotLabel& label) const {
        auto index = get_slot_index(label).value();
        assert(index < slots.size());
        return slots[index];
    }

    /// Retrieves the index (inside input or output slots) of the slot for the provided label.
    std::optional<size_t> get_slot_index(const SlotLabel& label) const {
        switch (label.type) {
            case SlotLabel::Type::Index:
                return std::make_optional(label.index);
            case SlotLabel::Type::Name: {
                auto iter = slots.begin();
                for (; iter < slots.end(); iter++) {
                    if (iter->name == label.name) {
                        return std::make_optional(iter - slots.begin());
                    }
                }
            }
        }

        return {};
    }
};

/// A value passed between render [`Nodes`](super::Node).
/// Corresponds to the [`SlotType`] specified in the [`RenderGraph`](super::RenderGraph).
///
/// Slots can have four different types of values:
/// [`Buffer`], [`TextureView`], [`Sampler`] and [`Entity`].
///
/// These values do not contain the actual render data, but only the ids to retrieve them.
struct SlotValue {
    SlotType type;

    union {
        VkBuffer buffer;
        VkImageView texture_view;
        VkSampler sampler;
        entt::entity entity;
    };
};

} // namespace Flint::Ecs

#endif // FLINT_GRAPH_SLOT_H
