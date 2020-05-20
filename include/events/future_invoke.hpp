//
// Created by sava on 11/14/19.
//

#ifndef __FUTURE_INVOKE_HPP_
#define __FUTURE_INVOKE_HPP_

namespace events
{
    using float_seconds = std::chrono::duration<float>;



    template<typename... TArgs>
    class future_invoke
    {
        using listener_type = listener<TArgs...>;

    public:
        future_invoke(
            float_seconds time,
            listener_type &listener_ref,
            std::tuple<TArgs...> args_tuple) :
            _time_left(time),
            _call([args_tuple, listener_ref] () {
                auto l = listener_ref;
                std::apply(l, args_tuple);
            }){}

        void call() const
        {
            _call();
        }

        inline void update(float_seconds dt)
        {
            _time_left -= dt;
        }

        [[nodiscard]] float_seconds time() const { return _time_left; }


    private:
        std::function<void()> _call;
        float_seconds _time_left{0};
    };

    template<typename... TArgs>
    class future_invoke_compare
    {
    public:
        bool operator()(future_invoke<TArgs...> a, future_invoke<TArgs...> b)
        {
            return a.time() < b.time();
        }

        static bool compare(const future_invoke<TArgs...>& a, const future_invoke<TArgs...>& b)
        {
            return a.time() > b.time();
        }
    };
}

#endif //__FUTURE_INVOKE_HPP_
