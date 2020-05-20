#ifndef __SCHENE_ENTITY_TREE_HPP_
#define __SCHENE_ENTITY_TREE_HPP_

#include <string>

#include <asset/scene.hpp>
#include <ecs/state.hpp>
#include <ecs/entity.hpp>
#include <asset/asset_loader_node.hpp>
#include <asset/component_loader.hpp>
#include <asset/scene.hpp>

namespace asset
{

	class scene_hydrater
	{
	public:
		scene_hydrater(ecs::state& ecs_state);

		template<typename... T>
		void register_loaders(T*... args)
		{
			_component_loaders.insert(_component_loaders.end(), { args... });
		}

		void populate_entities(asset::scene& scene);
		void load();
		ecs::entity& add_from_prototype(const std::string& path);
        void remove_entity(ecs::entity& entity);
        void remove_entity(entity_id id);
		void flush_removed();

		void clear();

	private:
		ecs::state& _ecs_state;
		scene* _scene{ nullptr };
		std::vector<asset_loader_node> _entity_refs;
		std::vector<component_loader*> _component_loaders;
		std::vector<ecs::entity*> _to_remove;

		void hydrate_recurse(asset_loader_node& graph_entity);
		void load_recurse(asset_loader_node& entity);
	};
}



#endif // !__SCHENE_ENTITY_TREE_HPP_
