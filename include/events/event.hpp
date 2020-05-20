//
// Created by sava on 10/20/19.
//

#ifndef __EVENT_HPP_
#define __EVENT_HPP_

#include <unordered_map>
#include <events/listener.hpp>
#include <atomic>
#include <vector>
#include <queue>
#include <chrono>
#include <tuple>

#include "future_invoke.hpp"

namespace events
{
    template<class... TArgs>
    class event
    {
        using listener_type = listener<TArgs...>;
        using future=future_invoke<TArgs...>;
        using future_compare=future_invoke_compare<TArgs...>;
        using float_seconds = std::chrono::duration<float>;

        static const size_t DelayQueueSize = 512;

    public:
        event()
        {
            _delayed.reserve(DelayQueueSize);
        }

        template <typename TFunc>
        listener_id add_listener(TFunc f)
        {
            listener_id id = ++next_listener_id;            
            _listeners.emplace(std::piecewise_construct,
                std::forward_as_tuple(id),
                std::forward_as_tuple(std::function<void(TArgs...)>(f), id));

            return id;
        }

        void remove_listener(listener_id id)
        {
            _listeners.erase(id);
        }

        void invoke(TArgs... args)
        {
            if (_listeners.empty()) return;

            for (auto &pair : _listeners)
            {
                listener<TArgs...>& l = pair.second;
                l(args...);
            }
        }

        void invoke_delayed(float_seconds time, TArgs... args)
        {
            if (_listeners.empty()) return;

            for (auto &pair : _listeners)
            {
                listener<TArgs...>& l = pair.second;

                _delayed.emplace_back(time, l, std::forward_as_tuple(args...));
                std::push_heap(_delayed.begin(), _delayed.end(), future_compare::compare);
            }
        }

        void update(float_seconds dt)
        {
            for(auto& v : _delayed)
                v.update(dt);

            while (!_delayed.empty())
            {
                auto& front = _delayed.front();
                if (front.time() > dt) break;

                front.call();

                std::pop_heap (_delayed.begin(),_delayed.end(), future_compare::compare);
                _delayed.pop_back();
            }
        }


    private:
        std::unordered_map<listener_id, listener_type> _listeners = {};
        std::vector<future> _delayed;
        future_compare future_invoke_comparator;

        static std::atomic_uint next_listener_id;
    };

    template<class... TArgs>
    std::atomic_uint event<TArgs...>::next_listener_id(0);
}
#endif //__EVENT_HPP_
