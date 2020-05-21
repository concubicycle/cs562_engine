#ifndef __RENDER_SYSTEM_HPP_
#define __RENDER_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <renderer/model_instance.hpp>

namespace renderer
{
	class render_system : public ecs::system_base
	{
	public:
		render_system();

		virtual void update(ecs::state& state) override;

	private:

		void initialize_opengl();
	};
}

#endif