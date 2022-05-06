#include "input_server.h"

namespace Flint {
    void InputServer::attach_callbacks(GLFWwindow *window) {
        // GLFW input callbacks.

        // A lambda function that doesn't capture anything can be implicitly converted to a regular function pointer.
        auto cursor_position_callback = [](GLFWwindow *window, double x_pos, double y_pos) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseMotion;
            input_event.args.mouse_motion.position = {(float) x_pos, (float) y_pos};
            auto &input_server = Flint::InputServer::get_singleton();
            input_server.cursor_position = {(float) x_pos, (float) y_pos};
            input_server.input_queue.push_back(input_event);
        };
        glfwSetCursorPosCallback(window, cursor_position_callback);

        auto cursor_button_callback = [](GLFWwindow *window, int button, int action, int mods) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseButton;
            input_event.args.mouse_button.button = button;
            input_event.args.mouse_button.pressed = action == GLFW_PRESS;
            auto &input_server = Flint::InputServer::get_singleton();
            input_event.args.mouse_button.position = input_server.cursor_position;
            input_server.input_queue.push_back(input_event);
        };
        glfwSetMouseButtonCallback(window, cursor_button_callback);
    }

    void InputServer::clear_queue() {
        input_queue.clear();
    }

    void InputEvent::consume() {
        consumed = true;
    }

    bool InputEvent::is_consumed() const {
        return consumed;
    }
}
