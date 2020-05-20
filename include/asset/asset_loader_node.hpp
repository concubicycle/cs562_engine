#ifndef __ENTITY_REF_NODE_HPP_
#define __ENTITY_REF_NODE_HPP_

#include <asset/scene_entity.hpp>
#include <ecs/entity.hpp>

namespace asset
{
	struct entity_ref_pair
	{
		ecs::entity* entity;
		const asset::scene_entity* entity_data;
	};

	struct asset_loader_node
	{
		asset_loader_node(ecs::entity& e, const asset::scene_entity& se) : entity_resource({ &e, &se }) {}

		entity_ref_pair entity_resource;
		std::vector<asset_loader_node> children{};
	};
}

#endif // !__ENTITY_REF_NODE_HPP_
