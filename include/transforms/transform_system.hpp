#ifndef __TRANSFORM_SYSTEM_HPP_
#define __TRANSFORM_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <transforms/transform.hpp>


namespace transforms
{
	class transform_system : public ecs::system_base
	{
	public:

		virtual void update(ecs::state& state) override;

	private:
		void update_transform(ecs::state& state, transform& t);
	};
}

#endif