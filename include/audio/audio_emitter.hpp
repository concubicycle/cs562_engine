//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_EMITTER_HPP
#define WIZARDENGINE_AUDIO_EMITTER_HPP

#include <cstdint>
#include <fmod.hpp>

#include <audio/sound_state.hpp>
#include <audio/emitter_sound.hpp>
#include <ecs/component.hpp>


namespace audio
{
    const std::uint32_t MaxSounds = 16;

    struct audio_emitter : ecs::component<audio_emitter>
    {
        std::uint16_t sound_count{0};
        emitter_sound emitter_sounds[MaxSounds];

        void add_sound(size_t hash)
        {
            emitter_sounds[sound_count++] = { hash };
        }

        void set_sound_state(std::uint32_t index, sound_state state)
        {
            emitter_sounds[index].state = state;
        }
    };
}

#endif //WIZARDENGINE_AUDIO_EMITTER_HPP
