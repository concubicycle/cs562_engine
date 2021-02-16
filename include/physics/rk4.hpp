#ifndef __RK4_HPP_
#define __RK4_HPP_

#include <Eigen/Core>

namespace physics
{
  struct rk4_data
  {
    rk4_data() = default;
    rk4_data(const rk4_data& other) = default;
    rk4_data& operator=(const rk4_data& other) = default;
    rk4_data(rk4_data&& other) noexcept = default;
    rk4_data& operator=(rk4_data&& other) = default;

    Eigen::Vector4f center_mass_position{ Eigen::Vector4f::Zero() };
    Eigen::Matrix3f rotation{ Eigen::Matrix3f::Identity() };
    Eigen::Vector4f linear_momentum{ Eigen::Vector4f::Zero() };
    Eigen::Vector4f angular_momentum{ Eigen::Vector4f::Zero() };

    rk4_data operator+(const rk4_data& other) const;
    rk4_data& operator+=(const rk4_data& delta);
    rk4_data operator-(const rk4_data& other) const;
    rk4_data& operator-=(const rk4_data& delta);
    rk4_data operator*(float value) const;
    rk4_data operator/(float value) const;
  };

  rk4_data operator*(float value, const rk4_data& rk4);
}

#endif