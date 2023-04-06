#ifndef FLINT_ECS_RENDER_GRAPH_H
#define FLINT_ECS_RENDER_GRAPH_H

#include <uuid.h>

#include <optional>
#include <unordered_map>

#include "../common/mat3x3.h"
#include "../common/quat.h"

using namespace Flint::Math;

namespace Flint::Ecs {

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

/// A [`Node`] which acts as an entry point for a [`RenderGraph`] with custom inputs.
/// It has the same input and output slots and simply copies them over when run.
struct GraphInputNode {
    std::vector<SlotInfo> inputs;
};

struct NodeId {
    uint64_t v;
};

struct Node;

/// An edge describing to ordering of both nodes (`output_node` before `input_node`)
/// and connecting the output slot at the `output_index` of the output_node
/// with the slot at the `input_index` of the `input_node`.
struct SlotEdge {
    NodeId input_node;
    size_t input_index;
    NodeId output_node;
    size_t output_index;
};

/// An edge describing to ordering of both nodes (`output_node` before `input_node`).
struct NodeEdge {
    NodeId input_node;
    NodeId output_node;
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
};

/// A collection of input or output [`SlotInfos`](SlotInfo) for
/// a [`NodeState`](super::NodeState).
struct SlotInfos {
    std::vector<SlotInfo> slots;
};

/// A collection of input and output [`Edges`](Edge) for a [`Node`].
struct Edges {
    NodeId id;
    std::vector<Edge> input_edges;
    std::vector<Edge> output_edges;
};

/// The internal representation of a [`Node`], with all data required
/// by the [`RenderGraph`](super::RenderGraph).
///
/// The `input_slots` and `output_slots` are provided by the `node`.
struct NodeState {
    NodeId id;
    std::optional<std::string> name;
    /// The name of the type that implements [`Node`].
    std::string type_name;
    std::unique_ptr<Node> node;
    SlotInfos input_slots;
    SlotInfos output_slots;
    Edges edges;
};

class RenderGraph {
private:
    std::unordered_map<NodeId, NodeState> nodes;
    std::unordered_map<std::string, NodeId> node_names;
    std::unordered_map<std::string, RenderGraph> sub_graphs;
    std::optional<NodeId> input_node;

public:
    /// The name of the [`GraphInputNode`] of this graph. Used to connect other nodes to it.
    const char* INPUT_NODE_NAME = "GraphInputNode";

    /// Updates all nodes and sub graphs of the render graph. Should be called before executing it.
    void update() {
        for (auto& node : nodes) {
            //                node.node.update(world);
        }

        for (auto& sub_graph : sub_graphs) {
            sub_graph.second.update();
        }
    }
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_RENDER_GRAPH_H
