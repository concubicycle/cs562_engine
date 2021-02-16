#ifndef __ENGINE_MATH_UTILS_HPP_
#define __ENGINE_MATH_UTILS_HPP_

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace math
{
  Eigen::Matrix3f cross_product_matrix(const Eigen::Vector3f& v);
}

#endif