#ifndef __MODEL_INSTANCE_HPP_
#define __MODEL_INSTANCE_HPP_

#include <ecs/component.hpp>

#include <renderer/opengl_model.hpp>
#include <renderer/opengl_material.hpp>

namespace renderer
{
	struct model_instance : public ecs::component<model_instance>
	{
		opengl_model model;		

		// hash of model path, so we can keep track of what model this originally was
		size_t model_file_hash{ 0 }; 

		bool is_closed_shape{ true };
	};
}


#endif