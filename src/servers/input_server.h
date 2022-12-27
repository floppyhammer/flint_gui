#ifndef FLINT_INPUT_SERVER_H
#define FLINT_INPUT_SERVER_H

#include "../common/geometry.h"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

using Pathfinder::Vec2;
using Pathfinder::Vec2F;

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
    BACKSPACE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class InputEvent {
public:
    InputEventType type = InputEventType::Max;

    union Args {
        struct {
            KeyCode key;
            bool pressed;
            bool repeated;
        } key{};
        struct {
            uint8_t button;
            bool pressed;
            Vec2F position;
        } mouse_button;
        struct {
            float x_delta;
            float y_delta;
        } mouse_scroll;
        struct {
            Vec2F relative;
            Vec2F position;
        } mouse_motion;
        struct {
            uint32_t codepoint;
        } text;
    } args{};

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
    Arrow,
    // The text input I-beam cursor shape.
    IBeam,
    // The crosshair shape.
    Crosshair,
    // The hand shape.
    Hand,
    // The horizontal resize arrow shape.
    ResizeH,
    // The vertical resize arrow shape.
    ResizeV,
    ResizeTlbr,
    ResizeTrbl,
};

class InputServer {
public:
    static InputServer *get_singleton() {
        static InputServer singleton;
        return &singleton;
    }

    InputServer();

    void init(GLFWwindow *window);

    void collect_events();

    Vec2F cursor_position;
    Vec2F last_cursor_position;

    std::vector<InputEvent> input_queue;

    GLFWwindow *current_window{};

    void set_cursor_captured(bool captured);

    void hide_cursor();

    void restore_cursor();

    void set_cursor(CursorShape shape);

private:
    GLFWcursor *arrow_cursor, *ibeam_cursor, *crosshair_cursor, *hand_cursor, *resize_cursor_h, *resize_cursor_v;
    GLFWcursor *resize_tlbr_cursor, *resize_trbl_cursor;
};

} // namespace Flint

#endif // FLINT_INPUT_SERVER_H
