#include "curve_tests.hpp"

#include <math/curve.hpp>

void test_b_spline()
{
  math::cubic_b_spline spline{ { 0.f, 0.f }, { 5.f, 10.f }, { 10.f, 10.f }, { 20.f, 0.f } };
  spline.add_piece({ 20.f, 10.f });
  spline.add_piece({ 10.f, 20.f });

  Eigen::Vector2f start = spline.eval(0);
  Eigen::Vector2f quart = spline.eval(0.25f);
  Eigen::Vector2f half = spline.eval(0.5);
  Eigen::Vector2f one = spline.eval(1);
  Eigen::Vector2f two = spline.eval(2);
  Eigen::Vector2f three = spline.eval(3);

  Eigen::Vector2f start_tan = spline.eval_tangent(0);

  std::cout << "b spline tests done" << std::endl;
}
