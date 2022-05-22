#include "input_server.h"

#include <locale>
#include <codecvt>

namespace Flint {
    std::string cpp11_codepoint_to_utf8(char32_t codepoint) {
        char utf8[4];
        char *end_of_utf8;

        char32_t const *from = &codepoint;

        std::mbstate_t mbs;
        std::codecvt_utf8<char32_t> ccv;

        if (ccv.out(mbs, from, from + 1, from, utf8, utf8 + 4, end_of_utf8)) {
            throw std::runtime_error("Bad codepoint-to-utf8 conversion!");
        }

        return {utf8, end_of_utf8};
    }

    // This should convert to whatever the system-wide character encoding
    // is for the platform (UTF-32/Linux - UCS-2/Windows)
    std::string ws_to_utf8(std::wstring const &s) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
        std::string utf8 = cnv.to_bytes(s);
        if (cnv.converted() < s.size()) {
            throw std::runtime_error("Incomplete wstring-to-utf8 conversion!");
        }
        return utf8;
    }

    std::wstring utf8_to_ws(std::string const &utf8) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
        std::wstring s = cnv.from_bytes(utf8);
        if (cnv.converted() < utf8.size()) {
            throw std::runtime_error("Incomplete utf8-to-wstring conversion!");
        }
        return s;
    }

    void InputServer::attach_callbacks(GLFWwindow *window) {
        // GLFW input callbacks.

        // A lambda function that doesn't capture anything can be implicitly converted to a regular function pointer.
        auto cursor_position_callback = [](GLFWwindow *window, double x_pos, double y_pos) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::MouseMotion;
            input_event.args.mouse_motion.position = {(float) x_pos, (float) y_pos};
            auto &input_server = Flint::InputServer::get_singleton();
            input_server.last_cursor_position = input_server.cursor_position;
            input_server.cursor_position = {(float) x_pos, (float) y_pos};
            input_event.args.mouse_motion.relative = input_server.cursor_position - input_server.last_cursor_position;
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

        auto character_callback = [](GLFWwindow *window, unsigned int codepoint) {
            Flint::InputEvent input_event{};
            input_event.type = Flint::InputEventType::Text;
            input_event.args.text.codepoint = codepoint;
            auto &input_server = Flint::InputServer::get_singleton();
            input_server.input_queue.push_back(input_event);
        };

        glfwSetCharCallback(window, character_callback);
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
