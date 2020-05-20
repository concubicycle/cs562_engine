#ifndef __RUNNING_AVERAGE_H_
#define __RUNNING_AVERAGE_H_

#include <cstdint>
#include <queue>

namespace util
{
    template <typename T, typename TDivisor = std::uint32_t>
    class running_average
    {
    public:
        running_average(std::uint32_t num_ints_tracked, T init_avg)
            :
            _average(init_avg),
            _last_total(_average* (TDivisor)num_ints_tracked),
            _num_ints_tracked(num_ints_tracked)
        {
            while (num_ints_tracked-- > 0)
                _past_values.push(_average);
        }

        // Updates the running average, and returns it
        T update(T value)
        {
            _last_total -= _past_values.front();
            _last_total += value;
            _past_values.pop();
            _past_values.push(value);
            _average = _last_total / (TDivisor)_num_ints_tracked;
            return _average;
        }

        T average() { return _average; }

    private:
        T _average;
        T _last_total;
        std::queue<T> _past_values;
        std::uint32_t _num_ints_tracked;
    };

} // namespace util

#endif