#include "node.h"

#include "graph.h"

namespace Flint::Ecs {

uint64_t NodeId::counter = 0;

std::vector<SlotInfo> Node::input() const {
    return {};
}

std::vector<SlotInfo> Node::output() const {
    return {};
}

void Node::update() {
}

NodeRunError Node::run(const RenderGraphContext& graph, RenderContext& render_context) const {
    return NodeRunError::None;
}

} // namespace Flint::Ecs
