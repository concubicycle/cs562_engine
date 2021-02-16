#ifndef __PHYS_POINT_MASS_HPP_
#define __PHYS_POINT_MASS_HPP_

#include <Eigen/Core>

namespace physics
{
  struct point_mass
  {
    Eigen::Vector3f position;
    float mass;

    point_mass(Eigen::Vector3f p, float m);
    Eigen::Matrix3f cross_product_matrix() const;
  };
}

#endif