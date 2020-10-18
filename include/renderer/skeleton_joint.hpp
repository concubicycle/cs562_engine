//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_NODE_HPP
#define WIZARDENGINE_SKELETON_NODE_HPP

#include <cstdint>
#include <vector>
#include <renderer/skeletal_animation.hpp>
#include <renderer/animation_time.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>


namespace renderer
{

    struct skeleton_joint
    {
        static const std::uint32_t MaxAnimations = 16;
        static const size_t UndefinedParent = -1;

        std::uint16_t bone_index;

        Eigen::Matrix4f base_transform;
        Eigen::Matrix4f offset;

        std::string name{ "unnamed" };
        size_t parent_index;
        Eigen::Affine3f inverse_bind_pose;

        skeletal_animation animations[MaxAnimations];        
        
        
        
        

        /**
         * Fills a buffer of bone matrices
         * @param parent_transform: the parent space, which is calculated at each frame of the recursion stack
         * @param t
         * @param animation_index
         * @param buffer
         */
        void compute_transform(
            Eigen::Matrix4f& global_inverse,
            const Eigen::Matrix4f& parent_transform,
            animation_time t,
            std::uint16_t animation_index,
            Eigen::Matrix4f* buffer);
    };

}
#endif //WIZARDENGINE_SKELETON_NODE_HPP
