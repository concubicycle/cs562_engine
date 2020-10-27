#ifndef __MATH_CURVE_HPP_
#define __MATH_CURVE_HPP_

#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <unordered_map>

namespace math
{
  // leave as header-only because will probably templatize later
  class cubic_b_spline
  {
  public:
    cubic_b_spline(
      Eigen::Vector2f a,
      Eigen::Vector2f b,
      Eigen::Vector2f c,
      Eigen::Vector2f d)
    {
      _bspline_mat <<
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 0, 3, 0,
        1, 4, 1, 0;

      _control_points.push_back(a);
      _control_points.push_back(b);
      _control_points.push_back(c);
      add_piece(d);
    }
    
    void add_piece(Eigen::Vector2f control_point)
    {
      _control_points.push_back(control_point);
      _max_s += 1;

      auto section_index = _control_points.size() - 4;

      Eigen::Matrix<float, 4, 2> points_matrix;
      points_matrix <<
        _control_points[section_index][0], _control_points[section_index][1],
        _control_points[section_index+1][0], _control_points[section_index + 1][1],
        _control_points[section_index+2][0], _control_points[section_index + 2][1],
        _control_points[section_index+3][0], _control_points[section_index + 3][1];

      Eigen::Matrix<float, 4, 2>& interp_mat = _interpolation_matrices.emplace_back();
      interp_mat = (1.f/6.f) * _bspline_mat * points_matrix;
    }

    Eigen::Vector2f eval(float s)
    {
      float whole;      
      float t = std::modf(s, &whole);

      if (s == _max_s) t = 1;

      float tsq = t * t;
      Eigen::Vector4f tvec(tsq * t, tsq, t, 1);

      auto interp_mat_idx = curve_data_index(s);
      Eigen::Matrix<float, 4, 2>& interpolation_matrix = _interpolation_matrices[interp_mat_idx];

      Eigen::Vector2f point = tvec.transpose() * interpolation_matrix;
      return point;
    }

    size_t max_s() const
    {
      return _max_s;
    }

  private:
    Eigen::Matrix4f _bspline_mat;
    std::vector<Eigen::Vector2f> _control_points;
    std::vector<Eigen::Matrix<float, 4, 2>> _interpolation_matrices;
    size_t _max_s{ 0 };

    size_t curve_data_index(float s)
    {
      s = std::min((float)_max_s, s);

      if (s == _max_s)
        return (size_t)(s - 1);
      
      return (size_t)s;
    }
  };

  class ark_length_table
  {
  public:
    ark_length_table(const cubic_b_spline& spline, size_t entry_count = 500)
    {
      
    }

  private:
    std::unordered_map<float, float> s_to_dist{ {0, 0} };
  };
}

#endif