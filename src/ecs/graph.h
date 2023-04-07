#ifndef FLINT_ECS_RENDER_GRAPH_H
#define FLINT_ECS_RENDER_GRAPH_H

#include <uuid.h>

#include <optional>
#include <unordered_map>

#include "../common/mat3x3.h"
#include "../common/quat.h"
#include "data.h"
#include "entt/entity/entity.hpp"

using namespace Flint::Math;

namespace Flint::Ecs {

struct RenderContext;

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

    // For hashing.
    inline bool operator<(const NodeId& rhs) const {
        return v < rhs.v;
    }
};

struct RenderGraphContext;

enum class NodeRunError {
    // Encountered an input slot error.
    InputSlotError,
    // Encountered an output slot error.
    OutputSlotError,
    // Encountered an error when running a sub-graph.
    RunSubGraphError,
};

/// A render node that can be added to a [`RenderGraph`](super::RenderGraph).
///
/// Nodes are the fundamental part of the graph and used to extend its functionality, by
/// generating draw calls and/or running subgraphs.
/// They are added via the `render_graph::add_node(my_node)` method.
///
/// To determine their position in the graph and ensure that all required dependencies (inputs)
/// are already executed, [`Edges`](Edge) are used.
///
/// A node can produce outputs used as dependencies by other nodes.
/// Those inputs and outputs are called slots and are the default way of passing render data
/// inside the graph. For more information see [`SlotType`](super::SlotType).
class Node {
public:
    /// Specifies the required input slots for this node.
    /// They will then be available during the run method inside the [`RenderGraphContext`].
    virtual auto input() -> std::vector<SlotInfo> {
        return {};
    }
    /// Specifies the produced output slots for this node.
    /// They can then be passed one inside [`RenderGraphContext`] during the run method.
    virtual auto output() -> std::vector<SlotInfo> {
        return {};
    }

    /// Updates internal node state using the current render [`World`] prior to the run method.
    virtual void update() {
    }

    /// Runs the graph node logic, issues draw calls, updates the output slots and
    /// optionally queues up subgraphs for execution. The graph data, input and output values are
    /// passed via the [`RenderGraphContext`].
    virtual auto run(const RenderGraphContext& graph, RenderContext& render_context) -> NodeRunError {
    }
};

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

    union {
        std::optional<SlotEdge> slot_edge;
        std::optional<NodeEdge> node_edge;
    };

    /// Returns the id of the `input_node`.
    NodeId get_input_node() const {
        if (type == Type::SlotEdge) {
            return slot_edge.value().input_node;
        } else {
            return node_edge.value().input_node;
        }
    }

    /// Returns the id of the `output_node`.
    NodeId get_output_node() const {
        if (type == Type::SlotEdge) {
            return slot_edge.value().output_node;
        } else {
            return node_edge.value().output_node;
        }
    }
};

/// A collection of input or output [`SlotInfos`](SlotInfo) for
/// a [`NodeState`](super::NodeState).
struct SlotInfos {
    std::vector<SlotInfo> slots;
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
        VkBuffer* buffer;
        VkImageView* texture_view;
        VkSampler* sampler;
        entt::entity* entity;
    };
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
    std::shared_ptr<Node> node;
    SlotInfos input_slots;
    SlotInfos output_slots;
    Edges edges;
};

class RenderGraph {
private:
    std::map<NodeId, NodeState> nodes;
    std::map<std::string, NodeId> node_names;
    std::map<std::string, RenderGraph> sub_graphs;
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

    /// Returns an iterator over the [`NodeStates`](NodeState).
    std::vector<std::reference_wrapper<const NodeState>> iter_nodes() const;

    /// Returns an iterator over a tuple of the input edges and the corresponding output nodes
    /// for the node referenced by the label.
    std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> iter_node_inputs(
        NodeId id) const;

    /// Returns an iterator over a tuple of the output edges and the corresponding input nodes
    /// for the node referenced by the label.
    std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> iter_node_outputs(
        NodeId id) const;

    /// Retrieves the [`NodeState`] referenced by the `label`.
    std::optional<std::reference_wrapper<const NodeState>> get_node_state(NodeId id) const;

    /// Returns the [`NodeState`] of the input node of this graph.
    ///
    /// # See also
    ///
    /// - [`input_node`](Self::input_node) for an unchecked version.
    std::optional<std::reference_wrapper<const NodeState>> get_input_node() const;

    /// Retrieves the sub graph corresponding to the `name`.
    std::optional<std::reference_wrapper<const RenderGraph>> get_sub_graph(const std::string& name) const;

    std::optional<std::reference_wrapper<RenderGraph>> get_sub_graph_mut(const std::string& name);
};

enum class RenderGraphRunnerError {
    None,
    NodeRunError,
    EmptyNodeOutputSlot,
    MissingInput,
    MismatchedInputSlotType,
    MismatchedInputCount,
};

struct RenderGraphRunner {
    RenderGraphRunnerError run(RenderGraph& graph);

    /// Run a graph.
    RenderGraphRunnerError run_graph(const RenderGraph& graph,
                                     const std::optional<std::string>& graph_name,
                                     RenderContext& render_context,
                                     const std::vector<SlotValue>& inputs,
                                     const std::optional<entt::entity> view_entity);
};

/// A command that signals the graph runner to run the sub graph corresponding to the `name`
/// with the specified `inputs` next.
struct RunSubGraph {
    std::string name;
    std::vector<SlotValue> inputs;
    std::optional<entt::entity> view_entity;
};

/// The context with all graph information required to run a [`Node`](super::Node).
/// This context is created for each node by the `RenderGraphRunner`.
///
/// The slot input can be read from here and the outputs must be written back to the context for
/// passing them onto the next node.
///
/// Sub graphs can be queued for running by adding a [`RunSubGraph`] command to the context.
/// After the node has finished running the graph runner is responsible for executing the sub graphs.
struct RenderGraphContext {
    const RenderGraph& graph;
    const NodeState& node;
    const std::vector<SlotValue>& inputs;
    std::vector<std::optional<SlotValue>>& outputs;
    std::vector<RunSubGraph> run_sub_graphs;
    /// The view_entity associated with the render graph being executed
    /// This is optional because you aren't required to have a view_entity for a node.
    /// For example, compute shader nodes don't have one.
    /// It should always be set when the RenderGraph is running on a View.
    std::optional<entt::entity> view_entity;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_RENDER_GRAPH_H
