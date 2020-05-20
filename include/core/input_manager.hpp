//
// Created by sava on 10/16/19.
//

#ifndef __INPUT_MANAGER_HPP_
#define __INPUT_MANAGER_HPP_

#include <optional>
#include <cstdint>
#include <cstring>


#include <util/running_average.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utility>

#include <Eigen/Core>


#define THUMBPAD_EPSILON 0.3f

namespace core
{
    class input_manager
    {
        std::uint8_t _glfw_mouse_button_codes[16];
        std::uint16_t _glfw_key_codes[128];

    public:
        input_manager(GLFWwindow *window);

        bool is_key_down(std::uint16_t key);

        bool was_key_pressed(std::uint16_t key);

        bool was_key_released(std::uint16_t key);

        bool was_mouse_clicked(std::uint8_t mouse_button);

        bool was_mouse_released(std::uint8_t mouse_button);

        void update();

        const Eigen::Vector2f mouse_delta() { return _mouse_delta.average(); }        

    private:
        GLFWwindow *_window;
        double _last_x, _last_y;
        util::running_average<Eigen::Vector2f, float> _mouse_delta;

        std::uint8_t* _current_key_states;
        std::uint8_t* _last_key_states;

        std::uint8_t* _current_mouse_button_states;
        std::uint8_t* _last_mouse_button_states;

        std::uint8_t _key_state_buffer_a[GLFW_KEY_LAST];
        std::uint8_t _key_state_buffer_b[GLFW_KEY_LAST];
        std::uint8_t _mouse_button_state_a[GLFW_MOUSE_BUTTON_8 +1];
        std::uint8_t _mouse_button_state_b[GLFW_MOUSE_BUTTON_8 +1];

        Eigen::Vector2f _last_click_position;

        static void (*build_mouse_callback(input_manager& r))(GLFWwindow* window, int, int, int)
        {
            static input_manager& manager = r;

            void (*callback)(GLFWwindow*, int, int, int) = ([](GLFWwindow* window, int button, int action, int mods) {
                (void)button;
                (void)action;
                (void)mods; // silence unused param warnings

                double x, y;
                glfwGetCursorPos(window, &x, &y);
                manager._last_click_position = {(float)x, (float)y};
            });

            return callback;
        }

        
    };
}

#endif //__INPUT_MANAGER_HPP_
