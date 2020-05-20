#ifndef __QUERY_CACHE_HPP_
#define __QUERY_CACHE_HPP_

#include <algorithm>
#include <vector>

#include <ecs/ecs_types.hpp>
#include <ecs/chunk_component_accessor.hpp>
#include <ecs/entity.hpp>

namespace ecs
{
	struct entity_accessor
	{
		entity_accessor(entity_id id, chunk_component_accessor a, component_bitset arch) 
			: entity(id)
			, accessor(a)
			, archetype(arch) {}

		entity_id entity;
		component_bitset archetype;
		chunk_component_accessor accessor;
	};

	struct query_cache
	{	
		component_bitset archetype;
		std::vector<entity_accessor> accessors{};
		bool is_sorted = false;

		void sort()
		{
			if (is_sorted) return;

			std::sort(accessors.begin(), accessors.end(),
				[](const entity_accessor& a, const entity_accessor& b) -> bool
				{
					return a.archetype > b.archetype;
				});

			is_sorted = true;
		}

		void remove(entity_id id)
		{
			accessors.erase(std::remove_if(
					accessors.begin(), 
					accessors.end(), 
					[id](entity_accessor& a) {
						return a.entity == id;
					}), 
				accessors.end());
		}
	};
}

#endif // !__QUERY_CACHE_HPP_
