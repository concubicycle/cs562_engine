#ifndef __FRAME_TIMER_H_
#define __FRAME_TIMER_H_

#include <cstdint>
#include <chrono>
#include <string>

#include <util/running_average.hpp>

using precision = std::chrono::nanoseconds;

namespace core
{
class frame_timer
{
private:
    util::running_average<std::chrono::nanoseconds> _frame_average;
    std::chrono::high_resolution_clock _high_res_timer;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start, _end, _begin_total;

    std::chrono::nanoseconds _delta;
    std::chrono::nanoseconds _smoothed_delta;


public:
    frame_timer() : 
		_frame_average(10, precision(0)),
		_smoothed_delta(precision(0)),
		_delta(precision(0))
	{}

	void begin_total();

    void start();
    void end();

    [[nodiscard]] std::chrono::nanoseconds delta() const;
    [[nodiscard]] std::chrono::nanoseconds smoothed_delta() const;
    [[nodiscard]] float smoothed_delta_secs() const;
    [[nodiscard]] float delta_secs() const;
	[[nodiscard]] std::chrono::nanoseconds current_frame_time() const;
	[[nodiscard]] float current_time_s() const;
	[[nodiscard]] std::string frame_info() const;
	[[nodiscard]] float total_s() const;
};
} // namespace core

#endif
