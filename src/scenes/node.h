//
// Created by chy on 6/2/2021.
//

#ifndef VULKAN_DEMO_APP_NODE_H
#define VULKAN_DEMO_APP_NODE_H

#include <vector>

namespace SimpleVulkanRenderer {
    class Node {
    private:
        std::vector<Node> children;

    public:
        virtual void self_update();

        virtual void self_draw();

        void update();

        void draw();
    };
}

#endif //VULKAN_DEMO_APP_NODE_H
