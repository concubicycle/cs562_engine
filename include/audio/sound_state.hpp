//
// Created by sava on 2020-02-29.
//

#ifndef WIZARDENGINE_SOUND_STATE_HPP
#define WIZARDENGINE_SOUND_STATE_HPP

namespace audio
{
    enum class sound_state
    {
        uninitialized = 0,
        unchanged = 1,
        started,
        playback_requested,
        stop_requested,
        playing,
        stopped
    };
}

#endif //WIZARDENGINE_SOUND_STATE_HPP
