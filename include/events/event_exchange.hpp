//
// Created by sava on 10/20/19.
//

#ifndef __EVENT_EXCHANGE_HPP_
#define __EVENT_EXCHANGE_HPP_

#include <events/event.hpp>
#include <events/event_maps.hpp>

namespace events
{
    /**
        TEventEnum: an underlying type for an event, usually some enum type. 
        TEvents: A list of instantiations of the event<> template:        
            events::event<float>,
            events::event<int>>,
            events::event<entity&>...        
    **/
    template <typename TEventEnum, typename... TEvents>
    class event_exchange
    {
    public:
        template <typename... TArgs, typename TFunc>
        listener_id subscribe(TEventEnum type, TFunc f)
        {
            auto& e = _event_maps.template find_event<TArgs...>(type);
            return e.add_listener(f);
        }
        
        void unsubscribe(TEventEnum type, listener_id id)
        {
            _event_maps.unsubscribe_all(type, id);
        }

        void update(float_seconds dt)
        {
            _event_maps.update_all(_event_maps, dt);
        }

        template <class... TArgs>
        void invoke(TEventEnum type, TArgs... args)
        {
            auto& e = _event_maps.template find_event<TArgs...>(type);
            e.invoke(args...);
        }
        
        template <class... TArgs>
        void invoke_delayed(TEventEnum type, float_seconds delay, TArgs... args)
        {
            auto& e = _event_maps.template find_event<TArgs...>(type);
            e.invoke_delayed(delay, args...);
        }

    private:
        event_maps<TEventEnum, TEvents...> _event_maps;       
    };
}
#endif //__EVENT_EXCHANGE_HPP_
