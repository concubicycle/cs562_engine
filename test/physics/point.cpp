#include "point.hpp"
#include <iostream>

constexpr float Epsilon = 0.0001f;

void test_com()
{
  physics::point_cloud points ({
    physics::point_mass({0.f, 0.f, 0.f}, 2.f),
    physics::point_mass({1.f, 0.f, 0.f}, 2.f),
    physics::point_mass({0.f, 1.f, 0.f}, 2.f),
    physics::point_mass({1.f, 1.f, 0.f}, 2.f)
  });

  Eigen::Vector3f com = points.center_of_mass();

  for (size_t i = 0; i < 2; ++i)
  {
    if (std::abs(com[i] - 0.5f) > Epsilon) throw std::runtime_error("wrong com");
  }

  points.bring_com_to_origin();
  com = points.center_of_mass();

  for (size_t i = 0; i < 2; ++i)
  {
    if (std::abs(com[i]) > Epsilon) throw std::runtime_error("wrong com");
  }

  std::cout << points << std::endl;
}
