#include <physics/point_cloud.hpp>
#include <numeric>
#include <iostream>
#include <Eigen/Dense>



physics::point_cloud::point_cloud()
{
  _inertial_tensor_obj = Eigen::Matrix3f::Zero();
}

physics::point_cloud::point_cloud(std::initializer_list<point_mass> l)
  : _points(l)
{
  calculate_inertial_tensor_obj();
}


Eigen::Vector3f physics::point_cloud::center_of_mass() const
{
  const float mass_sum = point_accumulate(0.f,
    [](const float sum, const point_mass& p) { return sum + p.mass; });

  return (1.f / mass_sum) * point_accumulate(Eigen::Vector3f(0, 0, 0),
    [](const Eigen::Vector3f& sum, const point_mass& p) { return sum + p.position * p.mass; });
}

void physics::point_cloud::bring_com_to_origin()
{
  const Eigen::Vector3f com = center_of_mass();
  for (auto& p : _points)
  {
    p.position -= com;
  }

  calculate_inertial_tensor_obj();
}

std::vector<physics::point_mass>& physics::point_cloud::get_points()
{
  _is_dirty = true;
  return _points;
}

const std::vector<physics::point_mass>& physics::point_cloud::get_points() const
{
  return _points;
}

const Eigen::Matrix3f& physics::point_cloud::inertial_tensor_obj()
{
  recalculate_if_dirty();
  return _inertial_tensor_obj;
}

Eigen::Matrix3f physics::point_cloud::inertial_tensor(Eigen::Matrix3f& rot)
{
  recalculate_if_dirty();
  return rot * _inertial_tensor_obj * rot.transpose();
}

Eigen::Matrix3f physics::point_cloud::inertial_tensor_inverse(const Eigen::Matrix3f& rot)
{
  recalculate_if_dirty();
  return rot * _inertial_tensor_obj_inverse * rot.transpose();
}

void physics::point_cloud::add_point(point_mass pm)
{
  _points.emplace_back(pm);
  const auto ixx = pm.mass * (pm.position[1] * pm.position[1] + pm.position[2] * pm.position[2]);
  const auto iyy = pm.mass * (pm.position[0] * pm.position[0] + pm.position[2] * pm.position[2]);
  const auto izz = pm.mass * (pm.position[0] * pm.position[0] + pm.position[1] * pm.position[1]);
  const auto ixy = pm.mass * pm.position[0] * pm.position[1];
  const auto ixz = pm.mass * pm.position[0] * pm.position[2];
  const auto iyz = pm.mass * pm.position[1] * pm.position[2];

  Eigen::Matrix3f delta;
  delta <<
    ixx, -ixy, -ixz,
    -ixy, iyy, -iyz,
    -ixz, -iyz, izz;

  _inertial_tensor_obj += delta;
  _total_mass += pm.mass;
}

float physics::point_cloud::mass() const
{
  return _total_mass;
}

void physics::point_cloud::calculate_inertial_tensor_obj()
{
  const float ixx = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * (pm.position[1] * pm.position[1] + pm.position[2] * pm.position[2]); });

  const float iyy = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * (pm.position[0] * pm.position[0] + pm.position[2] * pm.position[2]); });

  const float izz = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * (pm.position[0] * pm.position[0] + pm.position[1] * pm.position[1]); });

  const float ixy = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * pm.position[0] * pm.position[1]; });

  const float ixz = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * pm.position[0] * pm.position[2]; });

  const float iyz = point_accumulate(0.f, [](float sum, const point_mass& pm) {
    return sum + pm.mass * pm.position[1] * pm.position[2]; });

  _inertial_tensor_obj <<
    ixx, -ixy, -ixz,
    -ixy, iyy, -iyz,
    -ixz, -iyz, izz;

  /*const Eigen::Matrix3f zero = Eigen::Matrix3f::Zero();
  const Eigen::Matrix3f brute = point_accumulate(zero, [](Eigen::Matrix3f& sum, const point_mass& pm) {
    const Eigen::Matrix3f r_tilde = pm.cross_product_matrix();
    return (sum + -pm.mass * r_tilde * r_tilde).eval();
  });
  const Eigen::Matrix3f brute_inv = brute.inverse().eval();
  std::cout << "Brute I_obj: " << std::endl << brute << std::endl;
  std::cout << "Brute I_obj_inv: " << std::endl << brute_inv << std::endl;*/


  _inertial_tensor_obj_inverse = _inertial_tensor_obj.inverse().eval();
}

void physics::point_cloud::recalculate_if_dirty()
{
  if (_is_dirty)
  {
    calculate_inertial_tensor_obj();
    _total_mass = recalculate_mass();

    _is_dirty = false;
  }
}

float physics::point_cloud::recalculate_mass() const
{
  return point_accumulate(0.f, [&](const float sum, const point_mass& p) { return sum + p.mass; });
}

std::ostream& physics::operator<<(std::ostream& os, point_cloud& pm)
{
  const Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "");

  os << "Points:" << std::endl;
  for (const auto& p : pm.get_points())
    os << "position: (" << p.position.format(CommaInitFmt) << "), mass: " << p.mass << std::endl;
  os << std::endl;

  os << "Center Of Mass:" << std::endl;
  os << pm.center_of_mass().format(CommaInitFmt) << std::endl;
  os << std::endl;

  os << "I_obj:" << std::endl;
  os << pm.inertial_tensor_obj() << std::endl;
  os << std::endl;

  os << "I_obj_inverse:" << std::endl;
  os << pm._inertial_tensor_obj_inverse << std::endl;
  os << std::endl;

  return os;
}

