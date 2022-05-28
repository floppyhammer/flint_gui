#ifndef FLINT_INPUT_SERVER_H
#define FLINT_INPUT_SERVER_H

#include "../common/geometry.h"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <cstdint>
#include <vector>

using Pathfinder::Vec2;

namespace Flint {
    enum class InputEventType {
        MouseButton = 0,
        MouseMotion,
        MouseScroll,
        Key,
        Text,
        Max,
    };

    enum class KeyCode {
        UNKNOWN,
        BACKSPACE
    };

    class InputEvent {
    public:
        InputEventType type = InputEventType::Max;

        union Args {
            struct {
                KeyCode key;
                bool pressed;
            } key{};
            struct {
                uint8_t button;
                bool pressed;
                Vec2<float> position;
            } mouse_button;
            struct {
                float delta;
            } mouse_scroll;
            struct {
                Vec2<float> relative;
                Vec2<float> position;
            } mouse_motion;
            struct {
                uint32_t codepoint;
            } text;
        } args;

        void consume();

        bool is_consumed() const;

    private:
        bool consumed = false;
    };

    /// Unicode codepoint to UTF8 string.
    std::string cpp11_codepoint_to_utf8(char32_t codepoint);

    /// wstring to UTF8 string.
    std::string ws_to_utf8(std::wstring const &s);

    /// UTF8 string to wstring.
    std::wstring utf8_to_ws(std::string const &utf8);

    enum class CursorShape {
        // The regular arrow cursor.
        ARROW,
        // The text input I-beam cursor shape.
        IBEAM,
        // The crosshair shape.
        CROSSHAIR,
        // The hand shape.
        HAND,
        // The horizontal resize arrow shape.
        HRESIZE,
        // The vertical resize arrow shape.
        VRESIZE,
    };

    class InputServer {
    public:
        static InputServer *get_singleton() {
            static InputServer singleton;
            return &singleton;
        }

        InputServer();

        void attach_callbacks(GLFWwindow *window);

        Vec2<float> cursor_position;
        Vec2<float> last_cursor_position;

        std::vector<InputEvent> input_queue;

        GLFWwindow *current_window{};

        void clear_queue();

        void set_cursor_captured(bool captured);

        void hide_cursor();

        void restore_cursor();

        void set_cursor(CursorShape shape);

    private:
        GLFWcursor *arrow_cursor, *ibeam_cursor, *crosshair_cursor, *hand_cursor, *resize_cursor_h, *resize_cursor_v;
    };
}

#endif //FLINT_INPUT_SERVER_H
