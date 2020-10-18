#ifndef __JOINT_POSE_HPP_
#define __JOINT_POSE_HPP_

#include <math/quat.hpp>
#include <Eigen/Core>

namespace renderer
{
  struct joint_pose
  {
    Eigen::Vector3f translation;
    math::quat<float> rotation;
    float scale;
  };
}

#endif