//
// Created by chy on 2/16/2022.
//

#include "node.h"

void SimpleVulkanRenderer::Node::update() {
    self_update();

    for (auto& child : children) {
        child.update();
    }
}

void SimpleVulkanRenderer::Node::draw() {
    self_draw();

    for (auto& child : children) {
        child.draw();
    }
}

void SimpleVulkanRenderer::Node::self_update() {

}

void SimpleVulkanRenderer::Node::self_draw() {

}
