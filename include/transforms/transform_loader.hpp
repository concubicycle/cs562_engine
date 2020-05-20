#ifndef __TRANSFORM_LOADER_HPP_
#define __TRANSFORM_LOADER_HPP_


#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <transforms/transform.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace transforms
{
    class transform_loader : public asset::component_loader
    {
    public:
        void load(asset::asset_loader_node& node);
        component_bitset components_to_load();

    private:
        void load_rotation(const json& json, Eigen::Quaternionf& rotation);
        Eigen::Quaternionf euler_to_quaternion(const Eigen::Vector3f euler) const;
    };
}

#endif