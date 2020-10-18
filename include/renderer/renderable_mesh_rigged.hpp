//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
#define WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP

#include <vector>
#include <cstdint>

#include <rendering/skeleton_joint.hpp>
#include <rendering/mesh_static.hpp>
#include <rendering/material_pbr.hpp>
#include <ecs/component.hpp>
#include <glm/mat4x4.hpp>

namespace rendering
{

    struct renderable_mesh_rigged : ecs::component<renderable_mesh_rigged>
    {
        static const std::uint32_t MaxBones = 64;

        //skeleton_joint bones[MaxBones];
        std::vector<skeleton_joint> bones;

        glm::mat4 global_inverse;
        mesh_static mesh;
        material_pbr material;
        animation_time t{0};
        skeleton_joint *root{nullptr};
        std::uint32_t animation_count{0}; // possibly unneeded
        std::uint16_t animation_index{0};

        /**
         * Fills a buffer of bone matrices         
         * @param t
         * @param animation_index
         * @param buffer
         */
        void compute_transform(glm::mat4 *buffer);

        [[nodiscard]] animation_time find_animation_duration(std::uint32_t index) const;
    };
}


#endif //WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
