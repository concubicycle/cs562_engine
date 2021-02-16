#include <physics/spring.hpp>

Eigen::Vector4f physics::spring::force(Eigen::Vector3f point) const
{
  Eigen::Vector4f ret;
  ret << -spring_constant * (point - position), 0;
  return ret;
}

Eigen::Vector4f physics::spring::damp_force(Eigen::Vector3f velocity) const
{
  Eigen::Vector4f ret;
  ret << -damper_coefficient * velocity, 0;
  return ret;
}
