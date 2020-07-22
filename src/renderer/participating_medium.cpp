#include <renderer/participating_medium.hpp>
#include <renderer/renderer_component_bits.hpp>

template<> const component_shift ecs::component<renderer::participating_medium>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::participating_medium
};

