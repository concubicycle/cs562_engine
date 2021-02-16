#ifndef __PHYS_MODEL_HPP_
#define __PHYS_MODEL_HPP_

#include <chrono>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "point_cloud.hpp"
#include "rk4.hpp"

namespace physics
{
  class model
  {
    using float_second = std::chrono::duration<float>;

  public:    
    model(std::initializer_list<point_mass> l);
    model(std::initializer_list<Eigen::Vector3f> positions, float total_mass);

    void add_force(const Eigen::Vector3f& f);
    void add_torque(const Eigen::Vector3f& f);

    void euler_step(float_second dt);
    void rk4_step(float_second dt);

    point_cloud& points();

    [[nodiscard]] Eigen::Vector3f position() const;
    [[nodiscard]] Eigen::Quaternionf rotation() const;
    [[nodiscard]] Eigen::Vector4f velocity() const;
    [[nodiscard]] Eigen::Vector3f angular_velocity() const;
    [[nodiscard]] float mass() const;
    [[nodiscard]] Eigen::Vector3f velocity_at_local(Eigen::Vector3f point) const;

    void set_position(const Eigen::Vector3f& p);
    void set_rotation(Eigen::Quaternionf& q);


  private:
    point_cloud _points;    
    
    rk4_data _rk4 {};
    Eigen::Vector4f _force { Eigen::Vector4f::Zero() };
    Eigen::Vector4f _torque { Eigen::Vector4f::Zero() };

    // omega. direction is axis of rotation, magnitude is speed.
    Eigen::Vector3f _angular_velocity { Eigen::Vector3f::Zero() };
    Eigen::Vector4f _linear_velocity { Eigen::Vector4f::Zero() };
    Eigen::Matrix3f _inertial_tensor_inverse { Eigen::Matrix3f::Zero() };

    float_second _time { 0 };

    rk4_data rk4_block_derivative(const rk4_data& block);
  };
}

#endif