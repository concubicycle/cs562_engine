//
// Created by sava on 10/16/19.
// Edited by Silvia Barbero 1/29/20
//

#include "core/input_manager.hpp"

#include <iostream>



bool core::input_manager::is_key_down(std::uint16_t key)
{
    return _current_key_states[key];
}

bool core::input_manager::was_key_pressed(std::uint16_t key)
{
    return !(_last_key_states[key]) &&
        _current_key_states[key];
}

bool core::input_manager::was_key_released(std::uint16_t key)
{
    return _last_key_states[key] != GLFW_RELEASE &&
        _current_key_states[key] == GLFW_RELEASE;
}

bool core::input_manager::was_mouse_clicked(std::uint8_t mouse_button)
{
    return !(_last_mouse_button_states[mouse_button]) &&
        _current_mouse_button_states[mouse_button];
}

bool core::input_manager::was_mouse_released(std::uint8_t mouse_button)
{
    return _last_mouse_button_states[mouse_button] != GLFW_RELEASE &&
        _current_mouse_button_states[mouse_button] == GLFW_RELEASE;
}

void core::input_manager::update()
{    
    if (_skip_frame) 
    {
        _skip_frame = false;
        memcpy(_last_key_states, _current_key_states, sizeof(std::uint8_t) * GLFW_KEY_LAST);
        memcpy(_last_mouse_button_states, _current_mouse_button_states, sizeof(std::uint8_t) * (GLFW_MOUSE_BUTTON_8 + 1));

        return;
    }

    std::uint8_t* dummy = _last_key_states;
    _last_key_states = _current_key_states;
    _current_key_states = dummy;

    dummy = _last_mouse_button_states;
    _last_mouse_button_states = _current_mouse_button_states;
    _current_mouse_button_states = dummy;

    for(int i = 0; _glfw_key_codes[i] != GLFW_KEY_LAST; ++i)
    {
        auto state = glfwGetKey(_window, _glfw_key_codes[i]);
        _current_key_states[_glfw_key_codes[i]] = (std::uint8_t) state;
    }

    for(int i = 0; _glfw_mouse_button_codes[i] != GLFW_MOUSE_BUTTON_LAST; ++i)
    {
        auto state = glfwGetMouseButton(_window, _glfw_mouse_button_codes[i]);
        _current_mouse_button_states[_glfw_mouse_button_codes[i]] = (std::uint8_t) state;
    }

    if (_is_mouse_disabled) return;

    double x, y;
    glfwGetCursorPos(_window, &x, &y);

    _mouse_delta.update(Eigen::Vector2f(x - _last_x, y - _last_y));
    _last_x = x;
    _last_y = y;
}

core::input_manager::input_manager(GLFWwindow *window) :
    _window(window),
    _mouse_delta(3, Eigen::Vector2f::Zero()),
    _current_key_states(_key_state_buffer_a),
    _last_key_states(_key_state_buffer_b),
    _current_mouse_button_states(_mouse_button_state_a),
    _last_mouse_button_states(_mouse_button_state_b),
    _glfw_mouse_button_codes {
        GLFW_MOUSE_BUTTON_1,
        GLFW_MOUSE_BUTTON_2,
        GLFW_MOUSE_BUTTON_3,
        GLFW_MOUSE_BUTTON_4,
        GLFW_MOUSE_BUTTON_5,
        GLFW_MOUSE_BUTTON_6,
        GLFW_MOUSE_BUTTON_7,
        GLFW_MOUSE_BUTTON_8
    },
    _glfw_key_codes {
        GLFW_KEY_SPACE,
        GLFW_KEY_APOSTROPHE,
        GLFW_KEY_COMMA,
        GLFW_KEY_MINUS,
        GLFW_KEY_PERIOD,
        GLFW_KEY_SLASH,
        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,
        GLFW_KEY_SEMICOLON,
        GLFW_KEY_EQUAL,
        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
        GLFW_KEY_LEFT_BRACKET,
        GLFW_KEY_BACKSLASH,
        GLFW_KEY_RIGHT_BRACKET,
        GLFW_KEY_GRAVE_ACCENT,
        GLFW_KEY_WORLD_1,
        GLFW_KEY_WORLD_2,
        GLFW_KEY_ESCAPE,
        GLFW_KEY_ENTER,
        GLFW_KEY_TAB,
        GLFW_KEY_BACKSPACE,
        GLFW_KEY_INSERT,
        GLFW_KEY_DELETE,
        GLFW_KEY_RIGHT,
        GLFW_KEY_LEFT,
        GLFW_KEY_DOWN,
        GLFW_KEY_UP,
        GLFW_KEY_PAGE_UP,
        GLFW_KEY_PAGE_DOWN,
        GLFW_KEY_HOME,
        GLFW_KEY_END,
        GLFW_KEY_CAPS_LOCK,
        GLFW_KEY_SCROLL_LOCK,
        GLFW_KEY_NUM_LOCK,
        GLFW_KEY_PRINT_SCREEN,
        GLFW_KEY_PAUSE,
        GLFW_KEY_F1,
        GLFW_KEY_F2,
        GLFW_KEY_F3,
        GLFW_KEY_F4,
        GLFW_KEY_F5,
        GLFW_KEY_F6,
        GLFW_KEY_F7,
        GLFW_KEY_F8,
        GLFW_KEY_F9,
        GLFW_KEY_F10,
        GLFW_KEY_F11,
        GLFW_KEY_F12,
        GLFW_KEY_F13,
        GLFW_KEY_F14,
        GLFW_KEY_F15,
        GLFW_KEY_F16,
        GLFW_KEY_F17,
        GLFW_KEY_F18,
        GLFW_KEY_F19,
        GLFW_KEY_F20,
        GLFW_KEY_F21,
        GLFW_KEY_F22,
        GLFW_KEY_F23,
        GLFW_KEY_F24,
        GLFW_KEY_F25,
        GLFW_KEY_KP_0,
        GLFW_KEY_KP_1,
        GLFW_KEY_KP_2,
        GLFW_KEY_KP_3,
        GLFW_KEY_KP_4,
        GLFW_KEY_KP_5,
        GLFW_KEY_KP_6,
        GLFW_KEY_KP_7,
        GLFW_KEY_KP_8,
        GLFW_KEY_KP_9,
        GLFW_KEY_KP_DECIMAL,
        GLFW_KEY_KP_DIVIDE,
        GLFW_KEY_KP_MULTIPLY,
        GLFW_KEY_KP_SUBTRACT,
        GLFW_KEY_KP_ADD,
        GLFW_KEY_KP_ENTER,
        GLFW_KEY_KP_EQUAL,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_LEFT_ALT,
        GLFW_KEY_LEFT_SUPER,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_RIGHT_ALT,
        GLFW_KEY_RIGHT_SUPER,
        GLFW_KEY_MENU,
        GLFW_KEY_LAST
    }
{
    memset(_key_state_buffer_a, 0, sizeof(_key_state_buffer_a));
    memset(_key_state_buffer_b, 0, sizeof(_key_state_buffer_b));
    memset(_mouse_button_state_a, 0, sizeof(_mouse_button_state_a));
    memset(_mouse_button_state_b, 0, sizeof(_mouse_button_state_b));

    glfwSetMouseButtonCallback(window, build_mouse_callback(*this));

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    _last_x = x;
    _last_y = y;
}

