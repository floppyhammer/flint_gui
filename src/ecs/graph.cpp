#include "graph.h"

#include "render.h"

namespace Flint::Ecs {

RenderGraphRunnerError RenderGraphRunner::run(RenderGraph& graph) {
    RenderContext render_context;

    // Run the main graph.
    // Render commands will be fed to the render context.
    auto error = run_graph(graph, {}, render_context, {}, {});

    if (error != RenderGraphRunnerError::None) {
        return error;
    }

    // queue.submit(render_context.finish());

    return RenderGraphRunnerError::None;
}

RenderGraphRunnerError RenderGraphRunner::run_graph(const RenderGraph& graph,
                                                    const std::optional<std::string>& graph_name,
                                                    RenderContext& render_context,
                                                    const std::vector<SlotValue>& _inputs,
                                                    const std::optional<entt::entity> view_entity) {
    std::map<NodeId, std::vector<SlotValue>> node_outputs;

    // Queue up nodes without inputs, which can be run immediately.
    std::vector<std::reference_wrapper<const NodeState>> node_queue;
    for (auto& node_state : graph.iter_nodes()) {
        if (node_state.get().input_slots.slots.empty()) {
            node_queue.push_back(node_state);
        }
    }

    // Pass inputs into the graph.
    if (graph.get_input_node().has_value()) {
        auto input_node = graph.get_input_node().value();

        std::vector<SlotValue> input_values;
        input_values.reserve(4);

        for (int i = 0; i < input_node.get().input_slots.slots.size(); i++) {
            auto& input_slot = input_node.get().input_slots.slots[i];

            if (i < _inputs.size()) {
                auto& input_value = _inputs[i];
                if (input_slot.slot_type != input_value.type) {
                    // Mismatched input slot type.
                    abort();
                }
                input_values.push_back(input_value);
            } else {
                // Missing input.
                abort();
            }
        }

        node_outputs[input_node.get().id] = input_values;

        std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> output =
            graph.iter_node_outputs(input_node.get().id);

        if (output.has_value()) {
            for (auto& pair : output.value()) {
                node_queue.insert(node_queue.begin(), pair.second);
            }
        }
    }

    while (!node_queue.empty()) {
        bool continue_while_loop = false;

        auto node_state = node_queue.back();
        node_queue.pop_back();

        // Skip nodes that are already processed.
        if (node_outputs.find(node_state.get().id) != node_outputs.end()) {
            continue;
        }

        std::vector<std::pair<size_t, SlotValue>> slot_indices_and_inputs;

        // Check if all dependencies have finished running.
        auto node_inputs = graph.iter_node_inputs(node_state.get().id);
        if (node_inputs.has_value()) {
            for (auto& p : node_inputs.value()) {
                auto& edge = p.first;
                auto& input_node = p.second;

                switch (edge.type) {
                    case Edge::Type::SlotEdge: {
                        auto output_index = edge.slot_edge->output_index;
                        auto input_index = edge.slot_edge->input_index;

                        if (node_outputs.find(input_node.get().id) != node_outputs.end()) {
                            auto& outputs = node_outputs[input_node.get().id];
                            slot_indices_and_inputs.emplace_back(input_index, outputs[output_index]);
                        } else {
                            node_queue.insert(node_queue.begin(), node_state);

                            // Continue the outermost while loop.
                            continue_while_loop = true;
                            break;
                        }
                    }
                    case Edge::Type::NodeEdge: {
                        if (node_outputs.find(input_node.get().id) == node_outputs.end()) {
                            node_queue.insert(node_queue.begin(), node_state);
                            // Continue the outermost while loop.
                            continue_while_loop = true;
                            break;
                        }
                    }
                }
            }

            if (continue_while_loop) {
                continue;
            }
        } else {
            // Expect the node is in graph.
            abort();
        }

        struct by_key {
            bool operator()(std::pair<size_t, SlotValue> const& a, std::pair<size_t, SlotValue> const& b) const {
                return a.first < b.first;
            }
        };

        // Construct final sorted input list.
        std::sort(slot_indices_and_inputs.begin(), slot_indices_and_inputs.end(), by_key());

        std::vector<SlotValue> inputs;
        for (auto& p : slot_indices_and_inputs) {
            inputs.push_back(p.second);
        }

        if (inputs.size() != node_state.get().input_slots.slots.size()) {
            // Mismatched input count.
            abort();
        }

        std::vector<std::optional<SlotValue>> outputs(node_state.get().output_slots.slots.size());
        {
            auto context = RenderGraphContext{graph, node_state, inputs, outputs};

            if (view_entity.has_value()) {
                context.view_entity = view_entity;
            }

            { node_state.get().node->run(context, render_context); }

            for (auto& run_sub_graph : context.run_sub_graphs) {
                auto sub_graph = graph.get_sub_graph(run_sub_graph.name);

                if (!sub_graph.has_value()) {
                    // Sub graph should exist because it was validated when queued.
                    abort();
                }

                auto error = run_graph(sub_graph.value(),
                                       std::make_optional(run_sub_graph.name),
                                       render_context,
                                       run_sub_graph.inputs,
                                       run_sub_graph.view_entity);

                if (error != RenderGraphRunnerError::None) {
                    return error;
                }
            }
        }

        std::vector<SlotValue> values;
        for (int i = 0; i < outputs.size(); i++) {
            auto& output = outputs[i];
            if (output.has_value()) {
                values.push_back(output.value());
            } else {
                return RenderGraphRunnerError::EmptyNodeOutputSlot;
            }
        }
        node_outputs[node_state.get().id] = values;

        auto iter_node_outputs = graph.iter_node_outputs(node_state.get().id);
        if (iter_node_outputs.has_value()) {
            for (auto& p : iter_node_outputs.value()) {
                node_queue.insert(node_queue.begin(), p.second);
            }
        } else {
            // Node should exist.
            abort();
        }
    }

    return RenderGraphRunnerError::None;
}

/// Returns an iterator over the [`NodeStates`](NodeState).
std::vector<std::reference_wrapper<const NodeState>> RenderGraph::iter_nodes() const {
    std::vector<std::reference_wrapper<const NodeState>> node_states;

    node_states.reserve(nodes.size());

    for (auto& node : nodes) {
        node_states.emplace_back(node.second);
    }

    return node_states;
}

std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> RenderGraph::iter_node_inputs(
    NodeId id) const {
    auto node = get_node_state(id);

    if (!node.has_value()) {
        return {};
    }

    std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>> inputs;
    for (auto& edge : node.value().get().edges.input_edges) {
        inputs.emplace_back(edge, get_node_state(edge.get_output_node()).value());
    }

    return {std::move(inputs)};
}

std::optional<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>> RenderGraph::iter_node_outputs(
    NodeId id) const {
    auto node = get_node_state(id);

    if (!node.has_value()) {
        return {};
    }

    std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>> outputs;
    for (const auto& edge : node.value().get().edges.output_edges) {
        outputs.emplace_back(edge, get_node_state(edge.get_input_node()).value());
    }

    return {std::move(outputs)};
}

std::optional<std::reference_wrapper<const NodeState>> RenderGraph::get_node_state(NodeId id) const {
    auto iter = nodes.find(id);
    if (iter != nodes.end()) {
        return std::make_optional(std::cref(iter->second));
    }

    return {};
}

std::optional<std::reference_wrapper<const NodeState>> RenderGraph::get_input_node() const {
    if (input_node.has_value()) {
        return get_node_state(input_node.value());
    }

    return {};
}

std::optional<std::reference_wrapper<const RenderGraph>> RenderGraph::get_sub_graph(const std::string& name) const {
    auto iter = sub_graphs.find(name);
    if (iter != sub_graphs.end()) {
        return std::make_optional(std::cref(iter->second));
    } else {
        return {};
    }
}

std::optional<std::reference_wrapper<RenderGraph>> RenderGraph::get_sub_graph_mut(const std::string& name) {
    if (sub_graphs.find(name) != sub_graphs.end()) {
        return std::make_optional(std::ref(sub_graphs[name]));
    } else {
        return {};
    }
}

} // namespace Flint::Ecs
