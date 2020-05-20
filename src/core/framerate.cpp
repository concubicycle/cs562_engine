#include "core/framerate.hpp"

std::chrono::seconds core::framerate::seconds() const
{
    return _framerate;
}

std::chrono::nanoseconds core::framerate::frame_period() const
{
    return _frame_period;
}

std::chrono::nanoseconds core::framerate::calc_frametime(std::uint32_t target_framerate) const
{
    auto frame_time = std::chrono::duration<double>(1) / target_framerate;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(frame_time);
}