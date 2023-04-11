#include "edge.h"

#include "graph.h"

namespace Flint::Ecs {

NodeId Edge::get_input_node() const {
    if (type == Type::SlotEdge) {
        return slot_edge.value().input_node;
    } else {
        return node_edge.value().input_node;
    }
}
NodeId Edge::get_output_node() const {
    if (type == Type::SlotEdge) {
        return slot_edge.value().output_node;
    } else {
        return node_edge.value().output_node;
    }
}

bool Edges::has_output_edge(const Edge& edge) const {
    return std::find(output_edges.begin(), output_edges.end(), edge) != output_edges.end();
}
bool Edges::has_input_edge(const Edge& edge) const {
    return std::find(input_edges.begin(), input_edges.end(), edge) != input_edges.end();
}

Result<int, RenderGraphError> Edges::add_input_edge(const Edge& edge) {
    if (has_input_edge(edge)) {
        return {RenderGraphError::EdgeAlreadyExists};
    }
    input_edges.push_back(edge);
    return {0};
}

Result<int, RenderGraphError> Edges::add_output_edge(const Edge& edge) {
    if (has_output_edge(edge)) {
        return {RenderGraphError::EdgeAlreadyExists};
    }
    output_edges.push_back(edge);
    return {0};
}

} // namespace Flint::Ecs
