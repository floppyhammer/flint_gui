#ifndef FLINT_ECS_RENDER_GRAPH_H
#define FLINT_ECS_RENDER_GRAPH_H

#include <map>
#include <optional>

#include "edge.h"
#include "node.h"
#include "slot.h"

namespace Flint::Ecs {

template <typename C, typename E>
class Result {
public:
private:
    enum class Type {
        Ok,
        Error,
    } _type;

    C _content;
    E _error;

public:
    Result(C content) {
        _type = Type::Ok;
        _content = content;
    }

    Result(E error) {
        _type = Type::Error;
        _error = error;
    }

    bool is_ok() const {
        return _type == Type::Ok;
    }

    C unwrap() const {
        if (!is_ok()) {
            abort();
        }
        return _content;
    }

    E error() const {
        return _error;
    }
};

enum class RenderGraphError {
    None,
    InvalidNode,
    InvalidOutputNodeSlot,
    InvalidInputNodeSlot,
    WrongNodeType,
    MismatchedNodeSlots,
    EdgeAlreadyExists,
    EdgeDoesNotExist,
    UnconnectedNodeInputSlot,
    UnconnectedNodeOutputSlot,
    NodeInputSlotAlreadyOccupied,
};

/// The context with all information required to interact with the GPU.
///
/// The [`RenderDevice`] is used to create render resources and the
/// the [`CommandEncoder`] is used to record a series of GPU operations.
struct RenderContext {
    int a = 1;
    //                    command_encoder: Option<CommandEncoder>,
    //                                      command_buffers: Vec<CommandBuffer>,
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

    template <class NodeType>
    static NodeState from_node(NodeId id_, const std::shared_ptr<NodeType>& node_) {
        return NodeState{
            id_,
            {},
            typeid(NodeType).name(),
            node_,
            SlotInfos{node_->input()},
            SlotInfos{node_->output()},
            Edges{
                id_,
                {},
                {},
            },
        };
    }
};

/// A [`Node`] which acts as an entry point for a [`RenderGraph`] with custom inputs.
/// It has the same input and output slots and simply copies them over when run.
struct GraphInputNode {
    std::vector<SlotInfo> inputs;
};

class RenderGraphContext;

class RenderGraph {
private:
    std::map<NodeId, NodeState> _nodes;
    std::map<std::string, NodeId> _node_names;
    std::map<std::string, RenderGraph> _sub_graphs;
    std::optional<NodeId> _input_node;

public:
    /// The name of the [`GraphInputNode`] of this graph. Used to connect other nodes to it.
    const char* INPUT_NODE_NAME = "GraphInputNode";

    /// Updates all nodes and sub graphs of the render graph. Should be called before executing it.
    void update() {
        for (auto& node : _nodes) {
            //                node.node.update(world);
        }

        for (auto& sub_graph : _sub_graphs) {
            sub_graph.second.update();
        }
    }

    /// Returns an iterator over the [`NodeStates`](NodeState).
    std::vector<std::reference_wrapper<const NodeState>> iter_nodes() const;

    /// Returns an iterator over a tuple of the input edges and the corresponding output nodes
    /// for the node referenced by the label.
    std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> iter_node_inputs(
        const NodeLabel& label) const;

    /// Returns an iterator over a tuple of the output edges and the corresponding input nodes
    /// for the node referenced by the label.
    std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> iter_node_outputs(
        const NodeLabel& label) const;

    /// Retrieves the [`NodeId`] referenced by the `label`.
    std::pair<std::optional<NodeId>, RenderGraphError> get_node_id(const NodeLabel& label) const;

    /// Retrieves the [`NodeState`] referenced by the `label`.
    std::pair<std::optional<std::reference_wrapper<const NodeState>>, RenderGraphError> get_node_state(
        const NodeLabel& label) const;

    std::pair<std::optional<std::reference_wrapper<NodeState>>, RenderGraphError> get_node_state_mut(
        const NodeLabel& label);

    /// Adds the `node` with the `name` to the graph.
    /// If the name is already present replaces it instead.
    NodeId add_node(std::string name, const std::shared_ptr<Node>& node);

    /// Retrieves the [`Node`] referenced by the `label`.
    Result<std::shared_ptr<Node>, RenderGraphError> get_node(const NodeLabel& label);

    /// Adds the [`Edge::SlotEdge`] to the graph. This guarantees that the `output_node`
    /// is run before the `input_node` and also connects the `output_slot` to the `input_slot`.
    ///
    /// Fails if any invalid [`NodeLabel`]s or [`SlotLabel`]s are given.
    ///
    /// # See also
    ///
    /// - [`add_slot_edge`](Self::add_slot_edge) for an infallible version.
    RenderGraphError try_add_slot_edge(const NodeLabel& output_node,
                                       const SlotLabel& output_slot,
                                       const NodeLabel& input_node,
                                       const SlotLabel& input_slot);

    /// Adds the [`Edge::SlotEdge`] to the graph. This guarantees that the `output_node`
    /// is run before the `input_node` and also connects the `output_slot` to the `input_slot`.
    ///
    /// # Panics
    ///
    /// Any invalid [`NodeLabel`]s or [`SlotLabel`]s are given.
    ///
    /// # See also
    ///
    /// - [`try_add_slot_edge`](Self::try_add_slot_edge) for a fallible version.
    void add_slot_edge(const NodeLabel& output_node,
                       const SlotLabel& output_slot,
                       const NodeLabel& input_node,
                       const SlotLabel& input_slot);

    /// Adds the [`Edge::NodeEdge`] to the graph. This guarantees that the `output_node`
    /// is run before the `input_node`.
    ///
    /// Fails if any invalid [`NodeLabel`] is given.
    ///
    /// # See also
    ///
    /// - [`add_node_edge`](Self::add_node_edge) for an infallible version.
    RenderGraphError try_add_node_edge(const NodeLabel& output_node, const NodeLabel& input_node);

    /// Adds the [`Edge::NodeEdge`] to the graph. This guarantees that the `output_node`
    /// is run before the `input_node`.
    ///
    /// # Panics
    ///
    /// Panics if any invalid [`NodeLabel`] is given.
    ///
    /// # See also
    ///
    /// - [`try_add_node_edge`](Self::try_add_node_edge) for a fallible version.
    void add_node_edge(const NodeLabel& output_node_, const NodeLabel& input_node_);

    /// Add `node_edge`s based on the order of the given `edges` array.
    ///
    /// Defining an edge that already exists is not considered an error with this api.
    /// It simply won't create a new edge.
    void add_node_edges(std::vector<std::string> edges) {
        //        for window in edges.windows(2) {
        //        let [a, b] = window else { break; };
        //        if let Err(err) = self.try_add_node_edge(*a, *b) {
        //                match err {
        //                    // Already existing edges are very easy to produce with this api
        //                    // and shouldn't cause a panic
        //                    RenderGraphError::EdgeAlreadyExists(_) => {}
        //                    _ => panic!("{err:?}"),
        //                }
        //            }
        //        }
    }

    /// Returns the [`NodeState`] of the input node of this graph.
    ///
    /// # See also
    ///
    /// - [`input_node`](Self::input_node) for an unchecked version.
    std::optional<std::reference_wrapper<const NodeState>> get_input_node() const;

    /// Retrieves the sub graph corresponding to the `name`.
    std::optional<std::reference_wrapper<const RenderGraph>> get_sub_graph(const std::string& name) const;

    std::optional<std::reference_wrapper<RenderGraph>> get_sub_graph_mut(const std::string& name);

    /// Verifies that the edge existence is as expected and
    /// checks that slot edges are connected correctly.
    RenderGraphError validate_edge(const Edge& edge, EdgeExistence should_exist);

    bool RenderGraph::has_edge(const Edge& edge) const;
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

void run_graph_tests();

} // namespace Flint::Ecs

#endif // FLINT_ECS_RENDER_GRAPH_H
