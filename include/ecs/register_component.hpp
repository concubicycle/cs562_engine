// created by sava 

#ifndef __ADD_COMPONNET_HPP_
#define __ADD_COMPONNET_HPP_

#include <string>

#include <ecs/ecs_types.hpp>
#include <ecs/component_meta.hpp>

#include <util/string_table.hpp>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace ecs
{
    template <typename TComponent, std::uint32_t AllocationMin = component_meta::MinArchetypeChunks>
    void register_component(std::string type_name)
    {
        if (ecs::component_meta::bit_metas.find(TComponent::component_bitshift) !=
            ecs::component_meta::bit_metas.end())
        {
            return;
        }

        ecs::component_meta::bit_metas.insert(std::make_pair(
            TComponent::component_bitshift,
            ecs::component_meta::of<TComponent, AllocationMin>()));

        ecs::component_meta::type_to_bit.insert(std::make_pair(
            type_name,
            TComponent::component_bitshift));
    }
}


#endif