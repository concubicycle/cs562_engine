#include "linked_stick_system.hpp"
#include <transforms/transform.hpp>
#include "app_component_bits.hpp"


linked_stick_system::linked_stick_system(
  core::frame_timer& timer, 
  core::input_manager& input, 
  asset::scene_hydrater& hydrater)
  : _timer(timer)
  , _input(input)
  , _hydrater(hydrater)
{

}



void linked_stick_system::initialize(ecs::state& state)
{
  using namespace transforms;

  state.each<transform, linked_sticks>([&](transform& t, linked_sticks& ls)
    {
      float offset = ls.stick_length / 2.f;

      ls.springs.emplace_back(Eigen::Vector3f::Zero(), 0.1f, 6.f, 0.5f);

      for (size_t i = 0; i < ls.num_links; ++i)
      {
        auto& e = _hydrater.add_from_prototype(ls.stick_prefab);
        auto& e_t = e.get_component<transforms::transform>();
        ls.stick_markers.push_back(e.id());

        Eigen::Vector3f stick_pos = t.world_position();
        stick_pos[2] -= offset;
        e_t.position() = Eigen::Translation3f(stick_pos);
        offset += ls.stick_length;

        ls.stick_models.emplace_back(physics::model({
          physics::point_mass({-0.5f, -0.5f, -1.f}, 1.f),
          physics::point_mass({0.5f, -0.5f, -1.f}, 1.f),
          physics::point_mass({0.5f, -0.5f, 1.f}, 1.f),
          physics::point_mass({-0.5f, -0.5f, 1.f}, 1.f),
          physics::point_mass({-0.5f, 0.5f, -1.f}, 1.f),
          physics::point_mass({0.5f, 0.5f, -1.f}, 1.f),
          physics::point_mass({0.5f, 0.5f, 1.f}, 1.f),
          physics::point_mass({-0.5f, 0.5f, 1.f}, 1.f)
          }));

        ls.stick_models.back().set_position(stick_pos);

        Eigen::Vector3f spring_pos = stick_pos;
        spring_pos[2] -= ls.stick_length / 2.f;
        ls.springs.emplace_back(spring_pos, 0.1f, 6.f, 0.5f);
      }
    });
}

void linked_stick_system::update(ecs::state& state)
{
  using namespace transforms;

  state.each<transform, linked_sticks>([&](transform& t, linked_sticks& ls)
    {
      add_forces(state, t, ls);
      step_physics(state, t, ls);
      sync_transforms(state, t, ls);
      do_input(state, t, ls);
    });
}

// todo: oof
void linked_stick_system::add_forces(
  ecs::state& state,
  transforms::transform& t,
  linked_sticks& ls)
{
  using namespace transforms;

  // constants
  const auto half_len = ls.stick_length / 2.f;
  Eigen::Vector3f g(0, -20, 0);
  auto& spring_a = ls.springs[0];
  const auto k = 500;
  const auto d = 75;
  const auto M = ls.stick_models[0].mass();


  // first link
  {
    auto& model = ls.stick_models[0];
    auto& next_model = ls.stick_models[1];
    auto& stick = state.find_entity(ls.stick_markers[0]);
    auto& stick_t = stick.get_component<transform>();
    Eigen::Vector3f stick_fwd = (stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f stick_pos = stick_t.world_position();

    auto& next_stick = state.find_entity(ls.stick_markers[1]);
    auto& next_stick_t = next_stick.get_component<transform>();
    Eigen::Vector3f next_stick_fwd = (next_stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f next_stick_pos = next_stick_t.world_position();

    Eigen::Vector3f q_a = stick_pos - stick_fwd * half_len;
    Eigen::Vector3f q_b = stick_pos + stick_fwd * half_len;
    Eigen::Vector3f q_last_b = t.world_position();
    Eigen::Vector3f q_next_a = next_stick_pos - next_stick_fwd * half_len;

    Eigen::Vector3f v_a = model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_b = model.velocity_at_local({ 0, 0, -half_len });
    Eigen::Vector3f v_next_a = next_model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_next_b = next_model.velocity_at_local({ 0, 0, -half_len });
    Eigen::Vector3f v_last_a = Eigen::Vector3f::Zero();
    Eigen::Vector3f v_last_b = Eigen::Vector3f::Zero();
    Eigen::Vector3f f_a = -k * (q_a - q_last_b) - d * (v_a - v_last_b) + M / 2 * g;
    Eigen::Vector3f f_b = -k * (q_b - q_next_a) - d * (v_b - v_next_a) + M / 2 * g;

    model.add_force(f_a + f_b);

    Eigen::Vector3f tq_a = (q_a - stick_pos).cross(f_a);
    Eigen::Vector3f tq_b = (q_b - stick_pos).cross(f_b);
    model.add_torque(tq_a + tq_b);
  }

  // middle links
  for (size_t i = 1; i < ls.stick_markers.size() - 1; ++i)
  {
    auto& stick = state.find_entity(ls.stick_markers[i]);
    auto& stick_t = stick.get_component<transform>();
    auto& last_stick = state.find_entity(ls.stick_markers[i - 1]);
    auto& last_stick_t = last_stick.get_component<transform>();
    auto& next_stick = state.find_entity(ls.stick_markers[i + 1]);
    auto& next_stick_t = next_stick.get_component<transform>();

    Eigen::Vector3f stick_fwd = (stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f stick_pos = stick_t.world_position();
    Eigen::Vector3f q_a = stick_pos - stick_fwd * half_len;
    Eigen::Vector3f q_b = stick_pos + stick_fwd * half_len;

    Eigen::Vector3f last_stick_fwd = (last_stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f last_stick_pos = last_stick_t.world_position();
    Eigen::Vector3f q_last_a = last_stick_pos - last_stick_fwd * half_len;
    Eigen::Vector3f q_last_b = last_stick_pos + last_stick_fwd * half_len;

    Eigen::Vector3f next_stick_fwd = (next_stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f next_stick_pos = next_stick_t.world_position();
    Eigen::Vector3f q_next_a = next_stick_pos - next_stick_fwd * half_len;
    Eigen::Vector3f q_next_b = next_stick_pos + next_stick_fwd * half_len;

    auto& model = ls.stick_models[i];
    auto& last_model = ls.stick_models[i - 1];
    auto& next_model = ls.stick_models[i + 1];

    Eigen::Vector3f v_a = model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_b = model.velocity_at_local({ 0, 0, -half_len });
    Eigen::Vector3f v_last_a = last_model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_last_b = last_model.velocity_at_local({ 0, 0, -half_len });
    Eigen::Vector3f v_next_a = next_model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_next_b = next_model.velocity_at_local({ 0, 0, -half_len });

    Eigen::Vector3f f_a = -k * (q_a - q_last_b) - d * (v_a - v_last_b) + M / 2 * g;
    Eigen::Vector3f f_b = -k * (q_b - q_next_a) - d * (v_b - v_next_a) + M / 2 * g;

    model.add_force(f_a + f_b);

    Eigen::Vector3f tq_a = (q_a - stick_pos).cross(f_a);
    Eigen::Vector3f tq_b = (q_b - stick_pos).cross(f_b);
    model.add_torque(tq_a + tq_b);
  }

  // last link
  if (ls.stick_models.size() > 1)
  {
    size_t i = ls.stick_models.size() - 1;

    auto& stick = state.find_entity(ls.stick_markers[i]);
    auto& stick_t = stick.get_component<transform>();
    auto& last_stick = state.find_entity(ls.stick_markers[i - 1]);
    auto& last_stick_t = last_stick.get_component<transform>();

    Eigen::Vector3f stick_fwd = (stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f stick_pos = stick_t.world_position();
    Eigen::Vector3f q_a = stick_pos - stick_fwd * half_len;
    Eigen::Vector3f q_b = stick_pos + stick_fwd * half_len;

    Eigen::Vector3f last_stick_fwd = (last_stick_t.model_matrix() * Eigen::Vector4f(0, 0, -1, 0)).head<3>();
    Eigen::Vector3f last_stick_pos = last_stick_t.world_position();
    Eigen::Vector3f q_last_a = last_stick_pos - last_stick_fwd * half_len;
    Eigen::Vector3f q_last_b = last_stick_pos + last_stick_fwd * half_len;

    auto& model = ls.stick_models[i];
    auto& last_model = ls.stick_models[i - 1];

    Eigen::Vector3f v_a = model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_b = model.velocity_at_local({ 0, 0, -half_len });
    Eigen::Vector3f v_last_a = last_model.velocity_at_local({ 0, 0, half_len });
    Eigen::Vector3f v_last_b = last_model.velocity_at_local({ 0, 0, -half_len });

    Eigen::Vector3f f_a = -k * (q_a - q_last_b) - d * (v_a - v_last_b) + M / 2 * g;
    Eigen::Vector3f f_b = M / 2 * g;

    model.add_force(f_a + f_b);

    Eigen::Vector3f tq_a = (q_a - stick_pos).cross(f_a);
    Eigen::Vector3f tq_b = (q_b - stick_pos).cross(f_b);

    model.add_torque(tq_a);
  }
}

void linked_stick_system::step_physics(
  ecs::state& state,
  transforms::transform& t,
  linked_sticks& ls)
{
  for (auto& m : ls.stick_models)
  {
    m.rk4_step(_timer.delta_seconds());
  }
}

void linked_stick_system::sync_transforms(
  ecs::state& state,
  transforms::transform& t,
  linked_sticks& ls)
{
  for (size_t i = 0; i < ls.stick_markers.size(); ++i)
  {
    auto& model = ls.stick_models[i];
    auto& marker = state.find_entity(ls.stick_markers[i]);
    auto& marker_t = marker.get_component<transforms::transform>();
    marker_t.position() = Eigen::Translation3f(model.position());
    marker_t.rotation() = model.rotation();
  }
}

void linked_stick_system::do_input(
  ecs::state& state,
  transforms::transform& t,
  linked_sticks& ls)
{
  auto& t_translation = t.position();

  if (_input.is_key_down(GLFW_KEY_UP))
  {
    t_translation = Eigen::Translation3f(0, 0, -0.1f) * t_translation;
  }
  if (_input.is_key_down(GLFW_KEY_DOWN))
  {
    t_translation = Eigen::Translation3f(0, 0, 0.1f) * t_translation;
  }
  else if (_input.is_key_down(GLFW_KEY_RIGHT))
  {
    t_translation = Eigen::Translation3f(0.1f, 0, 0) * t_translation;
  }
  else if (_input.is_key_down(GLFW_KEY_LEFT))
  {
    t_translation = Eigen::Translation3f(-0.1f, 0, 0) * t_translation;
  }
}


template<> const component_shift ecs::component<linked_sticks>::component_bitshift{
    static_cast<component_shift>(app_component_bits::linked_sticks)
};
