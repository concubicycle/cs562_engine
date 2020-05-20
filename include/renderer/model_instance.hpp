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
		size_t model_file_hash{ 0 };
	};
}


#endif