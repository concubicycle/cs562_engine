//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_RIGGED_VERTEX_HPP
#define WIZARDENGINE_RIGGED_VERTEX_HPP

#include <cstdint>
#include <limits>

#include <Eigen/Core>

namespace asset
{
    struct rigged_vertex
    {
        Eigen::Vector3f position = Eigen::Vector3f(0);
        Eigen::Vector3f normal = Eigen::Vector3f(0);
        Eigen::Vector2f tex_coord = Eigen::Vector2f(0);
        Eigen::Vector3f tangent = Eigen::Vector3f(0);
        Eigen::Vector3f bitangent = Eigen::Vector3f(0);

        Eigen::Vector4i bone_ids = Eigen::Vector4i(0);
        Eigen::Vector4f weights = Eigen::Vector4f(0);

        void add_weight(std::uint32_t bone_index, float weight)
        {
            if (_weight_count < 4)
            {
                bone_ids[_weight_count] = bone_index;
                weights[_weight_count++] = weight;
            } else
            {
                replace_smallest_weight(bone_index, weight);
            }
        }

    //private:
        std::uint32_t _weight_count {0};

        void replace_smallest_weight(std::uint32_t bone_index, float weight)
        {
            std::uint8_t min_ind {0};
            float min = std::numeric_limits<float>::max();

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                if (weights[i] < min)
                {
                    min_ind = i;
                    min = weights[i];
                }
            }

            if (weight > min)
            {
                weights[min_ind] = weight;
                bone_ids[min_ind] = bone_index;
            }
        }
    };

}

#endif //WIZARDENGINE_RIGGED_VERTEX_HPP
