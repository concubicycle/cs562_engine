#ifndef __TRANSFORM_SYSTEM_HPP_
#define __TRANSFORM_SYSTEM_HPP_

#include <vector>
#include <ecs/system_base.hpp>
#include <transforms/transform.hpp>


namespace transforms
{
	class transform_system : public ecs::system_base
	{
	public:
		virtual void initialize(ecs::state& state) override;
		virtual void update(ecs::state& state) override;

	private:
		void set_children_dirty(ecs::state& state, transform& t);
		void update_transform(ecs::state& state, transform& t);
	};
}

#endif