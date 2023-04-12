#include "graph.h"

#include <iostream>
#include <utility>

namespace Flint::Ecs {

Result<int, RenderGraphRunnerError> RenderGraphRunner::run(RenderGraph& graph) {
    RenderContext render_context;

    // Run the main graph.
    // Generated render commands will be stored in the render context.
    auto res = run_graph(graph, {}, render_context, {}, {});
    CHECK_RESULT_RETURN(res)

    // queue.submit(render_context.finish());

    return {0};
}

Result<int, RenderGraphRunnerError> RenderGraphRunner::run_graph(const RenderGraph& graph,
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
                    return {RenderGraphRunnerError::MismatchedInputSlotType};
                }
                input_values.push_back(input_value);
            } else {
                return {RenderGraphRunnerError::MissingInput};
            }
        }

        node_outputs[input_node.get().id] = input_values;

        auto output = graph.iter_node_outputs(NodeLabel::from_id(input_node.get().id));

        if (output.is_ok()) {
            for (auto& pair : output.unwrap()) {
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
        auto node_inputs = graph.iter_node_inputs(NodeLabel::from_id(node_state.get().id));
        if (!node_inputs.is_ok()) {
            // Expect the node is in graph.
            abort();
        }

        for (auto& p : node_inputs.unwrap()) {
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
            return {RenderGraphRunnerError::MismatchedInputCount};
        }

        std::vector<std::optional<SlotValue>> outputs(node_state.get().output_slots.slots.size());
        {
            auto context = RenderGraphContext{graph, node_state, inputs, outputs};

            if (view_entity.has_value()) {
                context.view_entity = view_entity;
            }

            auto node_res = node_state.get().node->run(context, render_context);
            if (!node_res.is_ok()) {
                return {RenderGraphRunnerError::NodeRunError};
            }

            for (auto& run_sub_graph : context.run_sub_graphs) {
                auto sub_graph = graph.get_sub_graph(run_sub_graph.name);

                if (!sub_graph.has_value()) {
                    // Sub graph should exist because it was validated when queued.
                    abort();
                }

                auto res = run_graph(sub_graph.value(),
                                     std::make_optional(run_sub_graph.name),
                                     render_context,
                                     run_sub_graph.inputs,
                                     run_sub_graph.view_entity);
                CHECK_RESULT_RETURN(res)
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

        auto iter_node_outputs = graph.iter_node_outputs(NodeLabel::from_id(node_state.get().id));
        if (iter_node_outputs.is_ok()) {
            for (auto& p : iter_node_outputs.unwrap()) {
                node_queue.insert(node_queue.begin(), p.second);
            }
        } else {
            // Node should exist.
            abort();
        }
    }

    return RenderGraphRunnerError::None;
}

void RenderGraph::update() {
    // Update nodes in this graph.
    // NB: val in the loop is a copy, so we have to access
    // the map element using the key.
    for (auto& [key, val] : _nodes) {
        _nodes[key].node->update();
    }

    // Update sub graphs.
    // NB: val in the loop is a copy, so we have to access
    // the map element using the key.
    for (auto& [key, val] : _sub_graphs) {
        _sub_graphs[key].update();
    }
}

NodeId RenderGraph::set_input(const std::vector<SlotInfo>& inputs) {
    assert(!_input_node.has_value() && "Graph already has an input node");

    auto id = add_node("GraphInputNode", std::make_shared<GraphInputNode>(inputs));
    _input_node = std::make_optional(id);
    return id;
}

std::vector<std::reference_wrapper<const NodeState>> RenderGraph::iter_nodes() const {
    std::vector<std::reference_wrapper<const NodeState>> node_states;

    node_states.reserve(_nodes.size());

    for (auto& node : _nodes) {
        node_states.emplace_back(node.second);
    }

    return node_states;
}

Result<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>, RenderGraphError>
RenderGraph::iter_node_inputs(const NodeLabel& label) const {
    auto node = get_node_state(label);
    CHECK_RESULT_RETURN(node)

    std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>> inputs;
    for (auto& edge : node.unwrap().value().get().edges.input_edges) {
        inputs.emplace_back(edge, get_node_state(NodeLabel::from_id(edge.get_output_node())).unwrap().value());
    }

    return {std::move(inputs)};
}

Result<std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>>, RenderGraphError>
RenderGraph::iter_node_outputs(const NodeLabel& label) const {
    auto node = get_node_state(label);
    CHECK_RESULT_RETURN(node)

    std::vector<std::pair<Edge, std::reference_wrapper<const NodeState>>> outputs;
    for (const auto& edge : node.unwrap().value().get().edges.output_edges) {
        outputs.emplace_back(edge, get_node_state(NodeLabel::from_id(edge.get_input_node())).unwrap().value());
    }

    return {std::move(outputs)};
}

Result<NodeId, RenderGraphError> RenderGraph::get_node_id(const NodeLabel& label) const {
    if (label.type == NodeLabel::Type::Id) {
        return {label.id};
    } else {
        auto iter = _node_names.find(label.name);
        if (iter != _node_names.end()) {
            return {iter->second};
        }
    }

    return {RenderGraphError::InvalidNode};
}

Result<std::optional<std::reference_wrapper<const NodeState>>, RenderGraphError> RenderGraph::get_node_state(
    const NodeLabel& label) const {
    auto node_id = get_node_id(label);
    CHECK_RESULT_RETURN(node_id)

    auto iter = _nodes.find(node_id.unwrap());
    if (iter != _nodes.end()) {
        return {std::make_optional(std::cref(iter->second))};
    }

    return {RenderGraphError::InvalidNode};
}

Result<std::optional<std::reference_wrapper<NodeState>>, RenderGraphError> RenderGraph::get_node_state_mut(
    const NodeLabel& label) {
    auto node_id = get_node_id(label);
    CHECK_RESULT_RETURN(node_id)

    auto iter = _nodes.find(node_id.unwrap());
    if (iter != _nodes.end()) {
        return {std::make_optional(std::ref(iter->second))};
    }

    return {RenderGraphError::InvalidNode};
}

Result<int, RenderGraphError> RenderGraph::try_add_slot_edge(const NodeLabel& output_node_,
                                                             const SlotLabel& output_slot_,
                                                             const NodeLabel& input_node_,
                                                             const SlotLabel& input_slot_) {
    auto output_node_id = get_node_id(output_node_);
    CHECK_RESULT_RETURN(output_node_id)
    auto input_node_id = get_node_id(input_node_);
    CHECK_RESULT_RETURN(input_node_id)

    auto output_node_state = get_node_state(output_node_);
    CHECK_RESULT_RETURN(output_node_state)
    const auto output_index = output_node_state.unwrap().value().get().output_slots.get_slot_index(output_slot_);
    if (!output_index.has_value()) {
        return {RenderGraphError::InvalidOutputNodeSlot};
    }

    auto input_node_state = get_node_state(input_node_);
    CHECK_RESULT_RETURN(input_node_state)
    const auto input_index = input_node_state.unwrap().value().get().input_slots.get_slot_index(input_slot_);
    if (!input_index.has_value()) {
        return {RenderGraphError::InvalidOutputNodeSlot};
    }

    auto edge = Edge::from_slot_edge({
        input_node_id.unwrap(),
        input_index.value(),
        output_node_id.unwrap(),
        output_index.value(),
    });

    auto res = validate_edge(edge, EdgeExistence::DoesNotExist);
    CHECK_RESULT_RETURN(res)

    auto output_node = get_node_state_mut(NodeLabel::from_id(output_node_id.unwrap()));
    CHECK_RESULT_RETURN(output_node)
    res = output_node.unwrap().value().get().edges.add_output_edge(edge);
    CHECK_RESULT_RETURN(res)

    auto input_node = get_node_state_mut(NodeLabel::from_id(input_node_id.unwrap()));
    CHECK_RESULT_RETURN(input_node)
    res = input_node.unwrap().value().get().edges.add_input_edge(edge);
    CHECK_RESULT_RETURN(res)

    return {0};
}

void RenderGraph::add_slot_edge(const NodeLabel& output_node,
                                const SlotLabel& output_slot,
                                const NodeLabel& input_node,
                                const SlotLabel& input_slot) {
    try_add_slot_edge(output_node, output_slot, input_node, input_slot).unwrap();
}

Result<int, RenderGraphError> RenderGraph::try_add_node_edge(const NodeLabel& output_node_,
                                                             const NodeLabel& input_node_) {
    auto output_node_id = get_node_id(output_node_);
    CHECK_RESULT_RETURN(output_node_id)
    auto input_node_id = get_node_id(input_node_);
    CHECK_RESULT_RETURN(input_node_id)

    auto edge = Edge::from_node_edge({input_node_id.unwrap(), output_node_id.unwrap()});

    auto res = validate_edge(edge, EdgeExistence::DoesNotExist);
    CHECK_RESULT_RETURN(res)

    auto output_node = get_node_state_mut(NodeLabel::from_id(output_node_id.unwrap()));
    CHECK_RESULT_RETURN(output_node)

    res = output_node.unwrap().value().get().edges.add_output_edge(edge);
    CHECK_RESULT_RETURN(res)

    auto input_node = get_node_state_mut(NodeLabel::from_id(input_node_id.unwrap()));
    CHECK_RESULT_RETURN(input_node)
    res = input_node.unwrap().value().get().edges.add_input_edge(edge);
    CHECK_RESULT_RETURN(res)

    return {0};
}

void RenderGraph::add_node_edge(const NodeLabel& output_node_, const NodeLabel& input_node_) {
    try_add_node_edge(output_node_, input_node_).unwrap();
}

std::optional<std::reference_wrapper<const NodeState>> RenderGraph::get_input_node() const {
    if (_input_node.has_value()) {
        return get_node_state(NodeLabel::from_id(_input_node.value())).unwrap();
    }

    return {};
}

std::optional<std::reference_wrapper<const RenderGraph>> RenderGraph::get_sub_graph(const std::string& name) const {
    auto iter = _sub_graphs.find(name);
    if (iter != _sub_graphs.end()) {
        return std::make_optional(std::cref(iter->second));
    } else {
        return {};
    }
}

std::optional<std::reference_wrapper<RenderGraph>> RenderGraph::get_sub_graph_mut(const std::string& name) {
    if (_sub_graphs.find(name) != _sub_graphs.end()) {
        return std::make_optional(std::ref(_sub_graphs[name]));
    } else {
        return {};
    }
}

/// Checks whether the `edge` already exists in the graph.
bool RenderGraph::has_edge(const Edge& edge) const {
    auto output_node_state = get_node_state(NodeLabel::from_id(edge.get_output_node()));
    auto input_node_state = get_node_state(NodeLabel::from_id(edge.get_input_node()));

    if (output_node_state.is_ok()) {
        auto& output_edges = output_node_state.unwrap().value().get().edges.output_edges;
        if (std::find(output_edges.begin(), output_edges.end(), edge) != output_edges.end()) {
            if (input_node_state.is_ok()) {
                auto& input_edges = input_node_state.unwrap().value().get().edges.input_edges;
                if (std::find(input_edges.begin(), input_edges.end(), edge) != input_edges.end()) {
                    return true;
                }
            }
        }
    }

    return false;
}

Result<int, RenderGraphError> RenderGraph::validate_edge(const Edge& edge, EdgeExistence should_exist) {
    if (should_exist == EdgeExistence::Exists && !has_edge(edge)) {
        return {RenderGraphError::EdgeDoesNotExist};
    } else if (should_exist == EdgeExistence::DoesNotExist && has_edge(edge)) {
        return {RenderGraphError::EdgeAlreadyExists};
    }

    switch (edge.type) {
        case Edge::Type::SlotEdge: {
            auto& slot_edge = edge.slot_edge.value();

            auto output_node_state = get_node_state(NodeLabel::from_id(slot_edge.output_node));
            CHECK_RESULT_RETURN(output_node_state)
            auto input_node_state = get_node_state(NodeLabel::from_id(slot_edge.input_node));
            CHECK_RESULT_RETURN(input_node_state)

            auto output_slot = output_node_state.unwrap().value().get().output_slots.get_slot(
                SlotLabel::from_index(slot_edge.output_index));
            if (!output_slot.has_value()) {
                return {RenderGraphError::InvalidOutputNodeSlot};
            }

            auto input_slot = input_node_state.unwrap().value().get().input_slots.get_slot(
                SlotLabel::from_index(slot_edge.input_index));
            if (!input_slot.has_value()) {
                return {RenderGraphError::InvalidInputNodeSlot};
            }

            NodeId current_output_node;
            auto& input_edges = input_node_state.unwrap().value().get().edges.input_edges;
            auto iter = input_edges.begin();

            bool found = false;
            for (; iter < input_edges.end(); iter++) {
                if (iter->type == Edge::Type::SlotEdge) {
                    current_output_node = iter->slot_edge->input_node;
                    found = true;
                }
            }

            if (found) {
                if (should_exist == EdgeExistence::DoesNotExist) {
                    return {RenderGraphError::NodeInputSlotAlreadyOccupied};
                }
            }

            if (output_slot.value().slot_type != input_slot.value().slot_type) {
                return {RenderGraphError::MismatchedNodeSlots};
            }
        }
        case Edge::Type::NodeEdge: { /* Nothing to validate here */
        } break;
    }

    return {true};
}

#define FLINT_TEST

#ifdef FLINT_TEST
class TestNode : public Node {
private:
    std::vector<SlotInfo> _inputs;
    std::vector<SlotInfo> _outputs;

public:
    TestNode(std::vector<SlotInfo> inputs, std::vector<SlotInfo> outputs) {
        _inputs = std::move(inputs);
        _outputs = std::move(outputs);
    }

    static auto from_new(size_t input_count, size_t output_count) -> std::shared_ptr<TestNode> {
        std::vector<SlotInfo> inputs;
        for (int i = 0; i < input_count; i++) {
            inputs.push_back({"in_" + std::to_string(i), SlotType::TextureView});
        }

        std::vector<SlotInfo> outputs;
        for (int i = 0; i < output_count; i++) {
            outputs.push_back({"out_" + std::to_string(i), SlotType::TextureView});
        }

        return std::make_shared<TestNode>(inputs, outputs);
    }

    std::vector<SlotInfo> input() const override {
        return _inputs;
    }

    std::vector<SlotInfo> output() const override {
        return _outputs;
    }

    //    NodeRunError run(const RenderGraphContext& graph, RenderContext& render_context) const override {
    //        return NodeRunError::None;
    //    }
};

std::set<NodeId> input_nodes(const std::string& name, const RenderGraph& graph) {
    std::set<NodeId> nodes;

    auto inputs = graph.iter_node_inputs(NodeLabel::from_name(name));

    for (auto& p : inputs.unwrap()) {
        nodes.insert(p.second.get().id);
    }

    return nodes;
}

std::set<NodeId> output_nodes(const std::string& name, const RenderGraph& graph) {
    std::set<NodeId> nodes;

    auto outputs = graph.iter_node_outputs(NodeLabel::from_name(name));

    for (auto& p : outputs.unwrap()) {
        nodes.insert(p.second.get().id);
    }

    return nodes;
}

void test_graph_edges() {
    auto graph = RenderGraph();
    auto a_id = graph.add_node("A", TestNode::from_new(0, 1));
    auto b_id = graph.add_node("B", TestNode::from_new(0, 1));
    auto c_id = graph.add_node("C", TestNode::from_new(1, 1));
    auto d_id = graph.add_node("D", TestNode::from_new(1, 0));

    graph.add_slot_edge(NodeLabel::from_name("A"),
                        SlotLabel::from_name("out_0"),
                        NodeLabel::from_name("C"),
                        SlotLabel::from_name("in_0"));
    graph.add_node_edge(NodeLabel::from_name("B"), NodeLabel::from_name("C"));
    graph.add_slot_edge(
        NodeLabel::from_name("C"), SlotLabel::from_index(0), NodeLabel::from_name("D"), SlotLabel::from_index(0));

    assert(input_nodes("A", graph).empty() && "A has no inputs");
    assert(output_nodes("A", graph) == std::set<NodeId>({c_id}) && "A outputs to C");

    assert(input_nodes("B", graph).empty() && "B has no inputs");
    assert(output_nodes("B", graph) == std::set<NodeId>({c_id}) && "B outputs to C");

    assert(input_nodes("C", graph) == std::set<NodeId>({a_id, b_id}) && "A and B input to C");
    assert(output_nodes("C", graph) == std::set<NodeId>({d_id}) && "C outputs to D");

    assert(input_nodes("D", graph) == std::set<NodeId>({c_id}) && "C inputs to D");
    assert(output_nodes("D", graph).empty() && "D has no outputs");
}

void test_get_node_typed() {
    struct MyNode : public Node {
        size_t value;

        MyNode(size_t value_) {
            value = value_;
        }
    };

    auto graph = RenderGraph();

    graph.add_node("A", std::make_shared<MyNode>(42));

    auto node = graph.get_node<MyNode>(NodeLabel::from_name("A")).unwrap();
    assert(node->value == 42 && "node value matches");

    auto result = graph.get_node<TestNode>(NodeLabel::from_name("A"));
    assert(result.error() == RenderGraphError::WrongNodeType && "expect a wrong node type error");
}

void test_slot_already_occupied() {
    auto graph = RenderGraph();

    graph.add_node("A", TestNode::from_new(0, 1));
    graph.add_node("B", TestNode::from_new(0, 1));
    graph.add_node("C", TestNode::from_new(1, 1));

    graph.add_slot_edge(
        NodeLabel::from_name("A"), SlotLabel::from_index(0), NodeLabel::from_name("C"), SlotLabel::from_index(0));
    assert(graph.try_add_slot_edge(NodeLabel::from_name("B"),
                                   SlotLabel::from_index(0),
                                   NodeLabel::from_name("C"),
                                   SlotLabel::from_index(0))
                   .error() == RenderGraphError::NodeInputSlotAlreadyOccupied &&
           "Adding to a slot that is already occupied should return an error");
}

void test_edge_already_exists() {
    auto graph = RenderGraph();

    graph.add_node("A", TestNode::from_new(0, 1));
    graph.add_node("B", TestNode::from_new(1, 0));

    graph.add_slot_edge(
        NodeLabel::from_name("A"), SlotLabel::from_index(0), NodeLabel::from_name("B"), SlotLabel::from_index(0));
    assert(graph.try_add_slot_edge(NodeLabel::from_name("A"),
                                   SlotLabel::from_index(0),
                                   NodeLabel::from_name("B"),
                                   SlotLabel::from_index(0))
                   .error() == RenderGraphError::EdgeAlreadyExists &&
           "Adding to a duplicate edge should return an error");
}

void test_add_node_edges() {
    struct SimpleNode : public Node {
        SimpleNode() = default;
    };

    auto graph = RenderGraph();
    auto a_id = graph.add_node("A", std::make_shared<SimpleNode>());
    auto b_id = graph.add_node("B", std::make_shared<SimpleNode>());
    auto c_id = graph.add_node("C", std::make_shared<SimpleNode>());

    graph.add_node_edges({"A", "B", "C"});

    assert(output_nodes("A", graph) == std::set({b_id}) && "A -> B");
    assert(input_nodes("B", graph) == std::set({a_id}) && "A -> B");
    assert(output_nodes("B", graph) == std::set({c_id}) && "B -> C");
    assert(input_nodes("C", graph) == std::set({b_id}) && "B -> C");
}

void run_graph_tests() {
    test_graph_edges();
    test_get_node_typed();
    test_slot_already_occupied();
    test_edge_already_exists();
    test_add_node_edges();

    std::cout << "Graph tests passed." << std::endl;
}
#endif

} // namespace Flint::Ecs
