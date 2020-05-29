#include <renderer/local_punctual_light.hpp>
#include <renderer/renderer_component_bits.hpp>


template<> const component_shift ecs::component<renderer::local_punctual_light>::component_bitshift{
	(component_shift)renderer::renderer_component_bits::local_punctual_light
};
