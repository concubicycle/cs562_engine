//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_COMPONENT_BITS_HPP
#define WIZARDENGINE_COMPONENT_BITS_HPP

#include <ecs/ecs_types.hpp>

namespace audio
{
    enum class component_bits : component_shift
    {
        audio_emitter = 17,
        audio_listener = 18,
        music_player = 19
    };
}

#endif //WIZARDENGINE_COMPONENT_BITS_HPP
