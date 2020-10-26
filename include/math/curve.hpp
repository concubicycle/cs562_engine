#ifndef __MATH_CURVE_HPP_
#define __MATH_CURVE_HPP_

#include <Eigen/Core>

namespace math
{
  struct control_point
  {
    Eigen::Vector2f point;
    Eigen::Vector2f tangent;
  };

  
}

#endif