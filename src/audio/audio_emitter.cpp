//
// Created by sava on 1/27/20.
//

#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
#include <audio/component_bits.hpp>
#include <audio/audio_emitter.hpp>

template<> const component_shift ecs::component<audio::audio_emitter>::component_bitshift {
        (component_shift)audio::component_bits::audio_emitter
};

