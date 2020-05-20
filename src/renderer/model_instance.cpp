#include <renderer/model_instance.hpp>
#include <renderer/renderer_component_bits.hpp>

template<> const component_shift ecs::component<renderer::model_instance>::component_bitshift {
        (component_shift)renderer::renderer_component_bits::model_instance
};

