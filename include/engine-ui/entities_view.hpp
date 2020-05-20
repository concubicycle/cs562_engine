#ifndef __ENTITIES_VIEW_HPP_
#define __ENTITIES_VIEW_HPP_

#include <list>
#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>
#include "ecs/state.hpp"
#include "transforms/transform.hpp"
#include <ecs/ecs_types.hpp>
#include "engine-ui/transform_tree.hpp"
#include <ecs/entity.hpp>

namespace engineui
{

	class entities_view : public view
	{
		static const std::uint32_t Width = 200;
		static const std::uint32_t Height = 200;

		static const std::uint32_t InspectorWidth = 264;
		static const std::uint32_t InspectorHeight = 264;

	public:
		entities_view(core::viewport& viewport, event::EventManager& events, ecs::state& r_ecs_state);

		void draw();

		void update(ecs::state state);

	private:
		std::list<entity_id> _entities;

		event::EventManager& _events;

		ecs::state& m_r_ecs_state;
		transform_tree tree;
		ecs::entity* _current_entity_selected;
	};

}

#endif
