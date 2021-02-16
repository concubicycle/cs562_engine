#include <math/cross_product_matrix.hpp>

Eigen::Matrix3f math::cross_product_matrix(const Eigen::Vector3f& v)
{
  const float x = v[0], y = v[1], z = v[2];
  Eigen::Matrix3f m;
  m << 
    0, -z, y,
    z, 0, -x,
    -y, x, 0;
  return m;
}
