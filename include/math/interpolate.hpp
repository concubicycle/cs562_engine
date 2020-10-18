#ifndef ___MATH_INTERPOLATE_HPP_
#define ___MATH_INTERPOLATE_HPP_

#include <Eigen/Core>
#include <Eigen/Dense>

namespace math
{
  template <typename Tval, typename Ttime>
  Tval lerp(const Tval& a, const Tval& b, Ttime t)
  {
    return a + t*(b-a);
  }

  
  Eigen::Vector3f lerp_scale(const Eigen::Vector3f& s0, const Eigen::Vector3f& s1, float t)
  {
    Eigen::Vector3f quot = s1.cwiseQuotient(s0);
    quot[0] = std::pow(quot[0], t);
    quot[1] = std::pow(quot[1], t);
    quot[2] = std::pow(quot[2], t);

    return s0.cwiseProduct(quot);
  }
}

#endif