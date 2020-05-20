//
// Created by sava on 10/20/19.
//

#ifndef __LISTENER_HPP_
#define __LISTENER_HPP_

#include <functional>


namespace events
{
    typedef std::uint32_t listener_id;

    template<class... TArgs>
    class listener
    {
    public:
        using func_type = std::function<void(TArgs...)>;

        listener(func_type func, listener_id id) :
            _func(func),
            _id(id) {}

        operator func_type() { return _func; }

        void operator()(TArgs... args)
        {
            _func(args...);
        }

    private:
        listener_id _id;
        func_type _func;
    };


}

#endif //__LISTENER_HPP_
