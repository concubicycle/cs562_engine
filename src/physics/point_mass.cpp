#include <physics/point_mass.hpp>
#include <math/cross_product_matrix.hpp>
#include <utility>


physics::point_mass::point_mass(Eigen::Vector3f p, const float m)
  : position(std::move(p))
  , mass(m)
{
}

Eigen::Matrix3f physics::point_mass::cross_product_matrix() const
{
  return math::cross_product_matrix(position);
}
