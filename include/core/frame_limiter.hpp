#ifndef __FRAME_LIMITER_H_
#define __FRAME_LIMITER_H_

#include <stdint.h>

#include <core/frame_timer.hpp>
#include <core/framerate.hpp>
#include <platform/sleeper.hpp>

namespace core
{
class frame_limiter
{

public:
    frame_limiter(
        const frame_timer &frame_timer,
        std::uint32_t target_framerate,
        os::sleeper& sleeper) 
            : _frame_timer(frame_timer)
            , _framerate(core::framerate(target_framerate))
            , _sleeper(sleeper)
    {
    }

    void wait_remainder() const;

private:
    const frame_timer &_frame_timer;
    core::framerate _framerate;
    os::sleeper& _sleeper;
};
} // namespace core

#endif
