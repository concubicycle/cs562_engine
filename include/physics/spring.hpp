#ifndef __PHYS_SPRING_HPP_
#define __PHYS_SPRING_HPP_

#include <Eigen/Core>
#include <utility>

namespace physics
{
  struct spring
  {  
    Eigen::Vector3f position;
    float length;
    float spring_constant;
    float damper_coefficient;

    spring(Eigen::Vector3f p, float l, float k, float d)
    : position(std::move(p))
    , length(l)
    , spring_constant(k)
    , damper_coefficient(d) {}

    [[nodiscard]] Eigen::Vector4f force(Eigen::Vector3f point) const;
    [[nodiscard]] Eigen::Vector4f damp_force(Eigen::Vector3f velocity) const;
  };
}

#endif