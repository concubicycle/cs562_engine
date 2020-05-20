//
// Created by sava on 2020-02-29.
//

#ifndef WIZARDENGINE_MUSIC_PLAYER_HPP
#define WIZARDENGINE_MUSIC_PLAYER_HPP

#include <cstdint>

#include <ecs/component.hpp>
#include "emitter_sound.hpp"
#include "sound_state.hpp"

namespace audio
{
    struct music_player : ecs::component<music_player>
    {
        static const std::uint32_t MaxSounds = 32;

        std::uint16_t track_count{0};
        emitter_sound tracks[MaxSounds];

        void add_sound(size_t hash)
        {
            tracks[track_count++] = {hash };
        }

        void set_sound_state(std::uint32_t index, sound_state state)
        {
            tracks[index].state = state;
        }
    };
}
#endif //WIZARDENGINE_MUSIC_PLAYER_HPP
