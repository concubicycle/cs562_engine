//
// Created by sava on 10/8/19.
//

#ifndef __ENTITY_WORLD_H_
#define __ENTITY_WORLD_H_

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>


#include <ecs/entity.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/archetype_id.hpp>
#include <ecs/query_cache.hpp>


namespace ecs
{
	/**
	 * This is the primary class for storing and querying entities.
	 */
	class state
	{
		static std::atomic_uint NextEntityId;

	public:
		state(archetype_pools& state);

		entity& add_entity(component_bitset archetype_id, entity_id id);
		entity& add_entity(component_bitset archetype_id);
		void remove_entity(entity& entity);
		entity& find_entity(entity_id id);
		bool has_entity(entity_id id);
		void free_all();

		template <typename... TComponents, typename TFunc>
		void each(TFunc callback)
		{
			auto arch_id = ecs::archetype_id<TComponents...>();
			auto& cache = find_query_cache(arch_id);			

			for (entity_accessor& a : cache.accessors)
			{
				callback((*(a.accessor.get_component<TComponents>()))...);
			}
		}

		template <typename... TComponents, typename TFunc>
		ecs::entity* first(TFunc predicate)
		{
			auto arch_id = ecs::archetype_id<TComponents...>();
			auto& cache = find_query_cache(arch_id);

			for (entity_accessor& a : cache.accessors)
				if (predicate((*(a.accessor.get_component<TComponents>()))...))
					return &(find_entity(a.entity));

			return nullptr;
		}

		template <typename... TComponents>
		ecs::entity* first()
		{
			auto arch_id = ecs::archetype_id<TComponents...>();
			auto& cache = find_query_cache(arch_id);

			for (entity_accessor& a : cache.accessors)				
				return &(find_entity(a.entity));

			return nullptr;
		}

		template <typename... TComponents, typename TFunc>
		void each_id(TFunc callback)
		{
			auto arch_id = ecs::archetype_id<TComponents...>();
			auto& cache = find_query_cache(arch_id);            

			for (entity_accessor& a : cache.accessors)
			{
				callback(a.entity, (*(a.accessor.get_component<TComponents>()))...);
			}
		}

		template <typename... TComponents>
		entity& add_entity(entity_id id)
		{
			auto archetype = ecs::archetype_id<TComponents...>();
			return add_entity(archetype, id);
		}

		template <typename... TComponents>
		entity& add_entity()
		{
			auto archetype = ecs::archetype_id<TComponents...>();
			return add_entity(archetype);
		}

	private:
		archetype_pools& _entity_pools;
		std::unordered_set<entity_id> _ids;
		std::unordered_map<component_bitset, query_cache> _caches;
		std::unordered_map<entity_id, entity> _entity_lookup;

		query_cache& find_query_cache(component_bitset archetype);
		query_cache build_query_cache(component_bitset archetype);
	};
}

#endif //__ENTITY_WORLD_H_
