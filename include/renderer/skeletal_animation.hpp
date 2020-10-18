//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_ANIMATION_HPP
#define WIZARDENGINE_SKELETON_ANIMATION_HPP

#include <vector>
#include <cstdint>
#include <Eigen/Core>
#include <math/quat.hpp>
#include <renderer/skeletal_animation_frame.hpp>
#include <renderer/animation_time.hpp>

namespace renderer
{
    struct skeletal_animation
    {
        static const std::uint32_t MaxAnimationFrames = 300;

        std::string name{ "unnamed" };

        animation_time duration{ 0 };
        keyframes<math::quat<float>, MaxAnimationFrames> rotation;
        keyframes<Eigen::Vector3f, MaxAnimationFrames> position;
        keyframes<Eigen::Vector3f, MaxAnimationFrames> scale;

        Eigen::Matrix4f eval(animation_time t);

        bool is_blank() const { return duration == animation_time::zero(); }
    };
}

#endif //WIZARDENGINE_SKELETON_ANIMATION_HPP
