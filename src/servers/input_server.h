#ifndef FLINT_INPUT_SERVER_H
#define FLINT_INPUT_SERVER_H

#include <pathfinder/prelude.h>

#include <cstdint>
#include <vector>

#include "../common/geometry.h"
#include "../render/base.h"

namespace Pathfinder {
class Window;
}

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
    Unknown = 0,
    LeftControl,
    LeftShift,
    C,
    V,
    X,
    Backspace,
    Left,
    Right,
    Up,
    Down,
    Delete,
};

class InputEvent {
public:
    InputEventType type = InputEventType::Max;
    GLFWwindow *window{};

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

    void initialize_window_callbacks(GLFWwindow *window);

    void clear_events();

    std::string get_clipboard(Pathfinder::Window *window);
    void set_clipboard(Pathfinder::Window *window, std::string text);

    Vec2F cursor_position;
    Vec2F last_cursor_position;

    std::vector<InputEvent> input_queue;

    void set_cursor_captured(Pathfinder::Window *window, bool captured);

    void hide_cursor(Pathfinder::Window *window);

    void restore_cursor(Pathfinder::Window *window);

    void set_cursor(Pathfinder::Window *window, CursorShape shape);

    bool is_key_pressed(KeyCode code) const;

private:
    GLFWcursor *arrow_cursor, *ibeam_cursor, *crosshair_cursor, *hand_cursor, *resize_cursor_h, *resize_cursor_v;
    GLFWcursor *resize_tlbr_cursor, *resize_trbl_cursor;

    std::set<KeyCode> keys_pressed;
};

} // namespace Flint

#endif // FLINT_INPUT_SERVER_H
