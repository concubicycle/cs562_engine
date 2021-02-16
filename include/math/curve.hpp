#ifndef __MATH_CURVE_HPP_
#define __MATH_CURVE_HPP_

#include <iostream>
#include <vector>
#include <list>
#include <Eigen/Core>
#include <map>
#include <algorithm>

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

      if (_control_points.size() >= 4)
      {
        _max_s += 1;

        auto section_index = _control_points.size() - 4;

        Eigen::Matrix<float, 4, 2> points_matrix;
        points_matrix <<
          _control_points[section_index][0], _control_points[section_index][1],
          _control_points[section_index + 1][0], _control_points[section_index + 1][1],
          _control_points[section_index + 2][0], _control_points[section_index + 2][1],
          _control_points[section_index + 3][0], _control_points[section_index + 3][1];

        Eigen::Matrix<float, 4, 2>& interp_mat = _interpolation_matrices.emplace_back();
        interp_mat = (1.f / 6.f) * _bspline_mat * points_matrix;
      }
    }

    Eigen::Vector2f eval(float s) const
    {
      assert(!_interpolation_matrices.empty());

      float whole;
      float t = std::modf(s, &whole);

      if (s == _max_s) t = 1;

      float tsq = t * t;
      Eigen::Vector4f tvec(tsq * t, tsq, t, 1);

      auto interp_mat_idx = curve_data_index(s);
      const Eigen::Matrix<float, 4, 2>& interpolation_matrix = _interpolation_matrices[interp_mat_idx];

      Eigen::Vector2f point = tvec.transpose() * interpolation_matrix;
      return point;
    }

    Eigen::Vector2f eval_tangent(float s)
    {
      assert(!_interpolation_matrices.empty());

      float whole;
      float t = std::modf(s, &whole);

      if (s == _max_s) t = 1;

      float tsq = t * t;
      Eigen::Vector4f tvec(3 * tsq * t, 2 * tsq, 1, 0);

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

    size_t curve_data_index(float s) const
    {
      s = _max_s < s ? _max_s : s; //= std::min((float)_max_s, s);

      if (s == _max_s)
        return (size_t)(s - 1.f);
      
      return (size_t)s;
    }
  };

  class arc_length_table
  {
  public:
    arc_length_table(const cubic_b_spline& spline, size_t entry_count = 500)
      : _spline(&spline)
    {
      recalculate_with_tolerance(0.0000001f);
    }

    float get_s_for_distance(float d)
    {
      if (d == 0) return 0;

      std::pair<float, float> a;
      std::pair<float, float> b;

      for (auto it = s_to_dist.begin(); it != s_to_dist.end(); it++)
      {
        if (d < it->second)
        {
          b = *it;
          it--;
          a = *it;
          break;
        }
      }

      float alpha = (d - a.second) / (b.second - a.second);
      return a.first + alpha * (b.first - a.first);
    }

    void recalculate_with_tolerance(float tolerance)
    {
      s_to_dist.clear();      

      std::list<std::pair<float, float>> segments{ };
      segments.emplace_back(0, _spline->max_s());

     /* float inc = _spline->max_s() / 10.f;
      float s = 0;
      while (s < _spline->max_s())
      {        
        segments.emplace_back(s, s + inc);
        s += inc;
      }*/
      
      while (!segments.empty())
      {
        auto seg = segments.front();
        segments.pop_front();

        auto sa = seg.first;
        auto sb = seg.second;
        auto sm = (sa + sb) / 2.f;

        auto a = (_spline->eval(sm) - _spline->eval(sa).eval()).norm();
        auto b = (_spline->eval(sb) - _spline->eval(sm).eval()).norm();
        auto c = (_spline->eval(sb) - _spline->eval(sa).eval()).norm();

        if ((a + b - c) < tolerance)
        {
          s_to_dist[sm] = s_to_dist[sa] + a;
          s_to_dist[sb] = s_to_dist[sa] + a + b;

          _max_d = s_to_dist[sb] > _max_d ? s_to_dist[sb] : _max_d;
        }
        else
        {
          segments.emplace_front(sm, sb);
          segments.emplace_front(sa, sm);
        }
      }
    }

    float max_d()
    {
      return _max_d;
    }

  private:
    std::map<float, float> s_to_dist{ {0, 0} };    
    const cubic_b_spline* _spline;
    float _max_d = 0;
  };
}

#endif
