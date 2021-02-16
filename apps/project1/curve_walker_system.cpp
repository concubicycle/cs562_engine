#include <iostream>
#include "curve_walker_system.hpp"
#include <renderer/rigged_model_instance.hpp>
#include <renderer/animator.hpp>
#include <transforms/transform.hpp>
#include "curve_walker.hpp"

#include <util/constants.hpp>

curve_walker_system::curve_walker_system(core::frame_timer& time, asset::scene_hydrater& hydrater)
  : _time(time)
  , _hydrater(hydrater)
{
}

void curve_walker_system::initialize(ecs::state& state)
{
  state.each<curve_walker>([&](curve_walker& cw) {
    cw.walkee = state.first<renderer::rigged_model_instance>();

    auto& curve = cw.curve;
    auto inc = (float)curve->max_s() / 50.f;
    auto s = 0.f;
    while (s < curve->max_s())
    {
      auto point = curve->eval(s);
      auto& mark = _hydrater.add_from_prototype("assets/prototypes/curve_mark.json");
      auto& trns = mark.get_component<transforms::transform>();
      trns.position() = Eigen::Translation3f(point.x(), 0, point.y());
      trns.scale() = Eigen::Vector3f(0.25f, 0.25f, 0.25f);
      s += inc;

      cw.pos = Eigen::Vector2f(trns.world_position().x(), trns.world_position().z());
      cw.last_pos = cw.pos;
    }
    });
}

void curve_walker_system::update(ecs::state& state)
{
  using namespace transforms;
  using namespace renderer;

  state.each<curve_walker>(
    [&](curve_walker& cw)
    {
      auto& walkee_transform = cw.walkee->get_component<transform>();
      auto& walkee_rigged_model = cw.walkee->get_component<renderer::rigged_model_instance>();
      auto t1 = cw.t_rampup;
      auto t2 = cw.stop_t;
      auto t3 = cw.end_t;
      auto& anim_v = walkee_rigged_model.animation_structures->global_timeline.playback_rate;

      cw.t += _time.smoothed_delta_seconds_f();
      if (cw.is_stopping && cw.t >= cw.end_t) return;

      if (cw.t < cw.t_rampup) anim_v = cw.t / t1;
      else if (cw.is_stopping) anim_v = (t3 - cw.t) / (t3 - t2);
      else anim_v = 1;

      auto scaled_r = cw.r * anim_v;

      auto distance_rampup = [&](float t) { return cw.r * (t * t) / (2.f * t1); };
      auto distance_constant = [&](float t) { return cw.r * (t - t1); };
      auto distance_rampdown = [&](float t) { return cw.r / (2 * (t3 - t2)) * (2 * t3 * t - t * t - 2 * t3 * t2 + t2 * t2); };

      if (cw.t < cw.t_rampup)
        cw.d = distance_rampup(cw.t);
      else if (!cw.is_stopping)
        cw.d = distance_rampup(cw.t_rampup) + distance_constant(cw.t);
      else
        cw.d = distance_rampup(cw.t_rampup) + distance_constant(cw.stop_t) + distance_rampdown(cw.t);

      if (cw.d > cw.ark_lengths->max_d())
        cw.s = cw.d = cw.t = 0;

      cw.s = cw.ark_lengths->get_s_for_distance(cw.d);      

      Eigen::Vector2f curve_val = cw.curve->eval(cw.s);
      walkee_transform.position() = Eigen::Translation3f(curve_val.x(), 0, curve_val.y());

      Eigen::Vector2f tangent = cw.curve->eval_tangent(cw.s);
      tangent.normalize();
      Eigen::Vector3f v(tangent.x(), 0, tangent.y());
      Eigen::Vector3f w = Eigen::Vector3f::UnitY().cross(v);
      Eigen::Vector3f u = w.cross(v);
      Eigen::Matrix3f rmat;
      rmat <<
        w[0], v[0], u[0],
        w[1], v[1], u[1],
        w[2], v[2], u[2];

      rmat = rmat * Eigen::AngleAxisf(-util::HalfPi, Eigen::Vector3f(1, 0, 0)).matrix();
      Eigen::Quaternionf q(rmat);
      walkee_transform.rotation() = q;
    });
}
