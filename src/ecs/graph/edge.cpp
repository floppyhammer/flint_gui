#include "edge.h"

#include "graph.h"

namespace Flint::Ecs {

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
