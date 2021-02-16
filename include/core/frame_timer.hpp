#ifndef __FRAME_TIMER_H_
#define __FRAME_TIMER_H_

#include <cstdint>
#include <chrono>
#include <string>

#include <util/running_average.hpp>

namespace core
{
  using clock_units = std::chrono::high_resolution_clock::duration;
  using float_seconds = std::chrono::duration<float>;

  class frame_timer
  {
  private:
    util::running_average<std::chrono::nanoseconds> _frame_average;
    std::chrono::high_resolution_clock _high_res_timer;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start, _end, _begin_total;
        
    clock_units _smoothed_delta;
    clock_units _delta;

  public:
    frame_timer() :
      _frame_average(10, clock_units(0)),
      _smoothed_delta(clock_units(0)),
      _delta(clock_units(0))
    {}

    void begin_total();

    void start();
    void end();

    [[nodiscard]] clock_units delta() const;
    [[nodiscard]] clock_units smoothed_delta() const;
    [[nodiscard]] float_seconds smoothed_delta_seconds() const;
    [[nodiscard]] float_seconds delta_seconds() const;
    [[nodiscard]] float smoothed_delta_seconds_f() const;
    [[nodiscard]] float delta_seconds_f() const;
    [[nodiscard]] std::chrono::nanoseconds current_frame_time() const;
    [[nodiscard]] std::string frame_info() const;
  };
} // namespace core

#endif
