#include <physics/model.hpp>
#include <math/cross_product_matrix.hpp>

physics::model::model(const std::initializer_list<point_mass> l)
  : _points(l)  
{
  _points.bring_com_to_origin();
}

physics::model::model(const std::initializer_list<Eigen::Vector3f> positions, float total_mass)  
{
  float m = 1.f / static_cast<float>(positions.size());
  std::transform(positions.begin(), positions.end(), _points.get_points().begin(),
    [m](const Eigen::Vector3f& p) { return point_mass(p, m); });

  _points.bring_com_to_origin();
}

void physics::model::add_force(const Eigen::Vector3f& f)
{
  _force[0] += f[0];
  _force[1] += f[1];
  _force[2] += f[2];
}

void physics::model::add_torque(const Eigen::Vector3f& f)
{  
  _torque[0] += f[0];
  _torque[1] += f[1];
  _torque[2] += f[2];
}

void physics::model::euler_step(float_second dt)
{
  const rk4_data delta = rk4_block_derivative(_rk4);
  _rk4 += delta * dt.count();
  _time += dt;
}

void physics::model::rk4_step(float_second dt)
{
  constexpr float sixth = 1.f/6.f;
  const float seconds = dt.count();

  const rk4_data k_1 = rk4_block_derivative(_rk4) * seconds;
  const rk4_data k_2 = rk4_block_derivative(_rk4 + (k_1 / 2.f)) * seconds;
  const rk4_data k_3 = rk4_block_derivative(_rk4 + (k_2 / 2.f)) * seconds;
  const rk4_data k_4 = rk4_block_derivative(_rk4 + k_3) * seconds;
  const rk4_data delta = sixth * (k_1 + 2.f * k_2 + 2.f * k_3 + k_4);
  _rk4 += delta;
  _time += dt;

  _linear_velocity = _rk4.linear_momentum / _points.mass();
  _torque = Eigen::Vector4f::Zero();
  _force = Eigen::Vector4f::Zero();
}

physics::point_cloud & physics::model::points()
{
  return _points;
}

Eigen::Vector3f physics::model::position() const
{
  return _rk4.center_mass_position.head<3>();
}

Eigen::Quaternionf physics::model::rotation() const
{
  return Eigen::Quaternionf(_rk4.rotation);
}

Eigen::Vector4f physics::model::velocity() const
{
  return _linear_velocity;
}

Eigen::Vector3f physics::model::angular_velocity() const
{
  return _angular_velocity;
}

float physics::model::mass() const
{
  return _points.mass();
}

Eigen::Vector3f physics::model::velocity_at_local(Eigen::Vector3f point) const
{
  return math::cross_product_matrix(_angular_velocity) * _rk4.rotation * point
    + _linear_velocity.head<3>();
}

void physics::model::set_position(const Eigen::Vector3f &p)
{
  _rk4.center_mass_position << p, 1;
}

void physics::model::set_rotation(Eigen::Quaternionf &q)
{
  _rk4.rotation = q.toRotationMatrix();
}

physics::rk4_data physics::model::rk4_block_derivative(const rk4_data& block)
{
  _inertial_tensor_inverse = _points.inertial_tensor_inverse(block.rotation);
  _angular_velocity = _inertial_tensor_inverse * block.angular_momentum.head<3>();  

  return {
    _linear_velocity,
    math::cross_product_matrix(_angular_velocity) * block.rotation,
    _force,
    _torque
  };
}

