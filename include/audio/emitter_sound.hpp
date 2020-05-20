//
// Created by sava on 2020-02-29.
//

#ifndef WIZARDENGINE_EMITTER_SOUND_HPP
#define WIZARDENGINE_EMITTER_SOUND_HPP

#include <fmod.hpp>
#include <audio/sound_state.hpp>

namespace audio
{
    struct emitter_sound
    {
        size_t path_hash {};
        bool loop { false };
        float volume{ 1.f };
        sound_state state { sound_state::uninitialized };

        FMOD::Sound * fmod_sound{};
        FMOD::Channel * fmod_channel{};

        void set_null()
        {
            fmod_sound = nullptr;
            fmod_channel = nullptr;
        }

        bool is_null()
        {
            return fmod_sound == nullptr || fmod_channel == nullptr;
        }
    };
}

#endif //WIZARDENGINE_EMITTER_SOUND_HPP
