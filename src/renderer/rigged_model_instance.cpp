#include <renderer/rigged_model_instance.hpp>
#include <renderer/renderer_component_bits.hpp>


template<> const component_shift ecs::component<renderer::rigged_model_instance>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::rigged_model_instance
};

