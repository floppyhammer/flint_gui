#ifndef FLINT_RENDER_GRAPH_CONTEXT_H
#define FLINT_RENDER_GRAPH_CONTEXT_H

#include "node.h"
#include "slot.h"

namespace Flint::Ecs {

class RenderGraph;
class NodeState;

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

/// The context with all information required to interact with the GPU.
///
/// The [`RenderDevice`] is used to create render resources and the
/// the [`CommandEncoder`] is used to record a series of GPU operations.
struct RenderContext {
    int a = 1;
    // command_encoder: Option<CommandEncoder>,
    // command_buffers: Vec<CommandBuffer>,
};

enum class RunSubGraphError {
    MissingSubGraph,
    SubGraphHasNoInputs,
    MissingInput,
    MismatchedInputSlotType,
};

enum class OutputSlotError {
    InvalidSlot,
    MismatchedSlotType,
};

enum InputSlotError {
    InvalidSlot,
    MismatchedSlotType,
};

} // namespace Flint::Ecs

#endif // FLINT_RENDER_GRAPH_CONTEXT_H
