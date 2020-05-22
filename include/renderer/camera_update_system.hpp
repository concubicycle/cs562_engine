#ifndef __CAMERA_UPDATER_HPP_
#define __CAMERA_UPDATER_HPP_

#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>

namespace renderer
{
	class camera_update_system : public ecs::system_base
	{
	public:
		camera_update_system(core::glfw_context& glfw);

		virtual void initialize(ecs::state& state) override;
		virtual void update(ecs::state& state) override;

	private:
		core::glfw_context& _glfw;
	};
}


#endif