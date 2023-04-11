#ifndef FLINT_GRAPH_EDGE_H
#define FLINT_GRAPH_EDGE_H

#include <optional>
#include <string>
#include <vector>

#include "node.h"
#include "utils.h"

namespace Flint::Ecs {

/// An edge describing to ordering of both nodes (`output_node` before `input_node`)
/// and connecting the output slot at the `output_index` of the output_node
/// with the slot at the `input_index` of the `input_node`.
struct SlotEdge {
    NodeId input_node;
    size_t input_index;
    NodeId output_node;
    size_t output_index;

    inline bool operator==(const SlotEdge& rhs) const {
        return input_node == rhs.input_node || input_index == rhs.input_index || output_node == rhs.output_node ||
               output_index == rhs.output_index;
    }
};

/// An edge describing to ordering of both nodes (`output_node` before `input_node`).
struct NodeEdge {
    NodeId input_node;
    NodeId output_node;

    inline bool operator==(const NodeEdge& rhs) const {
        return input_node == rhs.input_node || output_node == rhs.output_node;
    }
};

/// An edge, which connects two [`Nodes`](super::Node) in
/// a [`RenderGraph`](crate::render_graph::RenderGraph).
///
/// They are used to describe the ordering (which node has to run first)
/// and may be of two kinds: [`NodeEdge`](Self::NodeEdge) and [`SlotEdge`](Self::SlotEdge).
///
/// Edges are added via the `render_graph::add_node_edge(output_node, input_node)` and the
/// `render_graph::add_slot_edge(output_node, output_slot, input_node, input_slot)` methods.
///
/// The former simply states that the `output_node` has to be run before the `input_node`,
/// while the later connects an output slot of the `output_node`
/// with an input slot of the `input_node` to pass additional data along.
/// For more information see [`SlotType`](super::SlotType).
struct Edge {
    enum class Type {
        SlotEdge,
        NodeEdge,
    } type;

    std::optional<SlotEdge> slot_edge;
    std::optional<NodeEdge> node_edge;

    static Edge from_slot_edge(const SlotEdge& slot_edge_) {
        return {Type::SlotEdge, std::make_optional(slot_edge_), {}};
    }

    static Edge from_node_edge(const NodeEdge& node_edge_) {
        return {Type::NodeEdge, {}, std::make_optional(node_edge_)};
    }

    /// Returns the id of the `input_node`.
    NodeId get_input_node() const;

    /// Returns the id of the `output_node`.
    NodeId get_output_node() const;

    inline bool operator==(const Edge& rhs) const {
        if (type == rhs.type) {
            if (type == Type::SlotEdge) {
                return *slot_edge == *rhs.slot_edge;
            } else {
                return *node_edge == *rhs.node_edge;
            }
        }
        return false;
    }
};

enum class EdgeExistence {
    Exists,
    DoesNotExist,
};

enum class RenderGraphError;

/// A collection of input and output [`Edges`](Edge) for a [`Node`].
struct Edges {
    NodeId id;
    std::vector<Edge> input_edges;
    std::vector<Edge> output_edges;

    /// Checks whether the input edge already exists.
    bool has_input_edge(const Edge& edge) const;

    /// Checks whether the output edge already exists.
    bool has_output_edge(const Edge& edge) const;

    /// Adds an edge to the `input_edges` if it does not already exist.
    Result<int, RenderGraphError> add_input_edge(const Edge& edge);

    /// Adds an edge to the `output_edges` if it does not already exist.
    Result<int, RenderGraphError> add_output_edge(const Edge& edge);
};

} // namespace Flint::Ecs

#endif // FLINT_GRAPH_EDGE_H
