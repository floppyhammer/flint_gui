#include "edge.h"

#include "graph.h"

namespace Flint::Ecs {

RenderGraphError Edges::add_input_edge(const Edge& edge) {
    if (has_input_edge(edge)) {
        return RenderGraphError::EdgeAlreadyExists;
    }
    input_edges.push_back(edge);
    return RenderGraphError::None;
}

RenderGraphError Edges::add_output_edge(const Edge& edge) {
    if (has_output_edge(edge)) {
        return RenderGraphError::EdgeAlreadyExists;
    }
    output_edges.push_back(edge);
    return RenderGraphError::None;
}

} // namespace Flint::Ecs
