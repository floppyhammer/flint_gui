#ifndef FLINT_INPUT_EVENT_H
#define FLINT_INPUT_EVENT_H

#include "../common/math/vec2.h"

#include <cstdint>

namespace Flint {
    enum class InputEventType {
        MouseButton = 0,
        MouseMotion,
        MouseScroll,
        Key,
        Max,
    };

    enum class KeyCode {

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
                Vec2<double> position;
            } mouse_button;
            struct {
                double delta;
            } mouse_scroll;
            struct {
                Vec2<double> relative;
                Vec2<double> position;
            } mouse_motion;
        } args;
    };
}

#endif //FLINT_INPUT_EVENT_H
