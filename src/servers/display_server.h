#ifndef FLINT_DISPLAY_SERVER_H
#define FLINT_DISPLAY_SERVER_H

#include "../common/geometry.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

using Pathfinder::Vec2;
using Pathfinder::Vec2F;

namespace Flint {

class DisplayServer {
public:
    static DisplayServer *get_singleton() {
        static DisplayServer singleton;
        return &singleton;
    }

    DisplayServer();

    ~DisplayServer();

    GLFWwindow* create_window(Vec2I size, std::string title);
};

} // namespace Flint

#endif // FLINT_DISPLAY_SERVER_H
