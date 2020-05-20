#ifndef __FRAMERATE_H_
#define __FRAMERATE_H_

#include <chrono>
#include <cstdint>

namespace core
{
class framerate
{
public:
    framerate(std::uint32_t seconds)
        : _framerate(seconds), _frame_period(calc_frametime(seconds))
    {
    }

    std::chrono::seconds seconds() const;
    std::chrono::nanoseconds frame_period() const;

private:
    std::chrono::seconds _framerate;
    std::chrono::nanoseconds _frame_period;

    std::chrono::nanoseconds calc_frametime(std::uint32_t target_framerate) const;
};
} // namespace core

#endif
