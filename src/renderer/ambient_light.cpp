#include <renderer/ambient_light.hpp>
#include <renderer/renderer_component_bits.hpp>

template<> const component_shift ecs::component<renderer::ambient_light>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::ambient_light
};

