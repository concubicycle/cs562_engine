#include <physics/rk4.hpp>

physics::rk4_data physics::rk4_data::operator+(const rk4_data& other) const
{
  return {
    center_mass_position + other.center_mass_position,
    rotation + other.rotation,
    linear_momentum + other.linear_momentum,
    angular_momentum + other.angular_momentum
  };
}

physics::rk4_data& physics::rk4_data::operator+=(const rk4_data& delta)
{
  *this = *this + delta;
  return *this;
}

physics::rk4_data physics::rk4_data::operator-(const rk4_data& other) const
{
  return *this + (-1.f * other);
}

physics::rk4_data& physics::rk4_data::operator-=(const rk4_data& delta)
{
  *this = *this - delta;
  return *this;
}

physics::rk4_data physics::rk4_data::operator*(float value) const
{
  return {
    center_mass_position * value,
    rotation * value,
    linear_momentum * value,
    angular_momentum * value
  };
}

physics::rk4_data physics::rk4_data::operator/(float value) const
{
  return {
    center_mass_position / value,
    rotation / value,
    linear_momentum / value,
    angular_momentum / value
  };
}

physics::rk4_data physics::operator*(float value, const rk4_data& rk4)
{
  return rk4 * value;
}
