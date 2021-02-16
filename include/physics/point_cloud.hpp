#ifndef __POINT_CLOUD_HPP_
#define __POINT_CLOUD_HPP_

#include <algorithm>
#include <numeric>
#include <vector>
#include <Eigen/Core>

#include "point_mass.hpp"

namespace physics
{
  class point_cloud
  {
  public:
    point_cloud();
    point_cloud(std::initializer_list<point_mass> l);
    Eigen::Vector3f center_of_mass() const;
    void bring_com_to_origin();

    std::vector<point_mass>& get_points();
    const std::vector<point_mass>& get_points() const;

    const Eigen::Matrix3f& inertial_tensor_obj();
    Eigen::Matrix3f inertial_tensor(Eigen::Matrix3f& rot);
    Eigen::Matrix3f inertial_tensor_inverse(const Eigen::Matrix3f& rot);

    void add_point(point_mass pm);

    float mass() const;


  private:
    std::vector<point_mass> _points;
    Eigen::Matrix3f _inertial_tensor_obj;
    Eigen::Matrix3f _inertial_tensor_obj_inverse;

    float _total_mass{
      recalculate_mass()
    };

    bool _is_dirty{ false };

    void calculate_inertial_tensor_obj();
    void recalculate_if_dirty();
    float recalculate_mass() const;

    template <typename TRet, typename TFunc>
    TRet point_accumulate(TRet initial_value, TFunc f) const
    {
      return std::accumulate(_points.begin(), _points.end(), initial_value, f);
    }

    friend std::ostream& operator<<(std::ostream& os, point_cloud& pm);

  };

  std::ostream& operator<<(std::ostream& os, point_cloud& pm);
}

#endif