#include <renderer/camera.hpp>
#include <renderer/renderer_component_bits.hpp>

template<> const component_shift ecs::component<renderer::camera>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::camera
};

