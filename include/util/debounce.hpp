//
// Created by sava on 10/17/19.
//

#ifndef __DEBOUNCE_HPP_
#define __DEBOUNCE_HPP_

#include <functional>
#include <chrono>

using float_second = std::chrono::duration<float>;
using debounce_clock = std::chrono::steady_clock;

/**
 * Used to limit the frequency at which some function can be called.
 * @tparam TArgs The function call args
 */
template <typename ...TArgs>
class debounce
{
public:
    debounce(
            float_second interval,
            std::function<void(TArgs...)> action) :
            _interval(interval),
            _action(action) {}

    void operator()(TArgs... args)
    {
        auto now = debounce_clock::now();
        if (now - _last > _interval)
        {
            _action(args...);
            _last = now;
        }
    }

    void reset()
    {
        _last = debounce_clock::now();
    }

private:
    float_second _interval;
    debounce_clock::time_point _last;
    std::function<void(TArgs...)> _action;

};

#endif //__DEBOUNCE_HPP_
