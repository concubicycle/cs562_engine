//
// Created by sava on 2020-02-29.
//

#include <audio/music_player.hpp>
#include <audio/component_bits.hpp>


template<> const component_shift ecs::component<audio::music_player>::component_bitshift {
        (component_shift)audio::component_bits::music_player
};

