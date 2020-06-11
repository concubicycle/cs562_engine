#include <transforms/freefly_system.hpp>
#include <transforms/transform.hpp>
#include <transforms/freefly_component.hpp>

#include <util/constants.hpp>

transforms::freefly_system::freefly_system(core::input_manager& input, core::frame_timer& timer)
	: _input(input)
    , _timer(timer)
{
}

void transforms::freefly_system::update(ecs::state& state)
{
    state.each<transform, freefly_component>([&](transform& t, freefly_component& fc) {
        auto frame_time = _timer.smoothed_delta_secs();
        auto speed = fc.walk_speed;
        auto mouse_delta = _input.mouse_delta();

        auto& t_translation = t.position();
        auto& fc_rotation = fc.euler_angles;
        auto& t_rotation = t.rotation();

        Eigen::Matrix3f rot_mat = t.rotation().matrix();
        Eigen::Vector3f right = rot_mat.col(0);
        Eigen::Vector3f up = rot_mat.col(1);
        Eigen::Vector3f fwd = -rot_mat.col(2);

        Eigen::Vector2f rotation = Eigen::Vector2f(0.f, 0.f);
        rotation[1] -= mouse_delta[0] * frame_time * 0.1f;
        rotation[0] -= mouse_delta[1] * frame_time * 0.1f;

        fc_rotation[0] += rotation[0];
        fc_rotation[1] += rotation[1];

        if (fc_rotation[0] > util::HalfPi)
            fc_rotation[0] = util::HalfPi;
        if (fc_rotation[0] < -util::HalfPi)
            fc_rotation[0] = -util::HalfPi;

        t_rotation = Eigen::AngleAxisf(fc_rotation[2], Eigen::Vector3f::UnitZ())
            * Eigen::AngleAxisf(fc_rotation[1], Eigen::Vector3f::UnitY())
            * Eigen::AngleAxisf(fc_rotation[0], Eigen::Vector3f::UnitX());

        if (_input.is_key_down(GLFW_KEY_W))
        {
            t_translation = Eigen::Translation3f(fwd * frame_time * speed) * t_translation;
        }
        if (_input.is_key_down(GLFW_KEY_A))
        {
            t_translation = Eigen::Translation3f(-right * frame_time * speed) * t_translation;
        }
        else if (_input.is_key_down(GLFW_KEY_D))
        {
            t_translation = Eigen::Translation3f(right * frame_time * speed) * t_translation;
        }
        else if (_input.is_key_down(GLFW_KEY_S))
        {
            t_translation = Eigen::Translation3f(-fwd * frame_time * speed) * t_translation;
        }
	});
}