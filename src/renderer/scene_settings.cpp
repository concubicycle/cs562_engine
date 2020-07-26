#include <renderer/scene_settings.hpp>
#include <renderer/renderer_component_bits.hpp>

template<> const component_shift ecs::component<renderer::scene_settings>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::scene_settings
};

