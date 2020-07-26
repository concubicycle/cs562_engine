#ifndef __RENDERER_COMPONENT_BITS_HPP_
#define __RENDERER_COMPONENT_BITS_HPP_

namespace renderer
{
	enum class renderer_component_bits
	{
		model_instance = 4,
		punctual_light = 5,
		camera = 6,
		ambient_light = 7,
		local_punctual_light = 8,
		directional_light = 9,
		scene_settings = 10
	};
}


#endif