#ifndef __EVENT_MAPS_HPP_
#define __EVENT_MAPS_HPP_

#include <tuple>
#include <unordered_map>

#include <events/event.hpp>

namespace events
{
	template <typename TEventEnum, typename... TEvents>
	class event_maps
	{
        using tuple_type = std::tuple<
            std::unordered_map<TEventEnum, TEvents>...
        >;

	private:
		tuple_type _event_maps;

    public:
        template <class... TArgs>
        event<TArgs...>& find_event(TEventEnum type)
        {
            auto& events = std::get<std::unordered_map<TEventEnum, event<TArgs...>>>(_event_maps);
            auto it = events.find(type);
            if (it == events.end())
                events.insert(std::pair<TEventEnum, event<TArgs...>>(type, event<TArgs...>()));

            it = events.find(type);
            return it->second;
        }

        template<int index, typename... Ts>
        struct unsubscribe_all_recurse {
            void operator() (tuple_type& tuple, TEventEnum type, listener_id id)
            {
                auto& e_map = std::get<index>(tuple);
                auto it_type = e_map.find(type);
                if (it_type != e_map.end())
                {
                    it_type->second.remove_listener(id);
                    return;
                }
                unsubscribe_all_recurse<index - 1, Ts...>{}(tuple, type, id);
            }
        };

        template<typename... Ts>
        struct unsubscribe_all_recurse<0, Ts...> {
            void operator() (tuple_type& tuple, TEventEnum type, listener_id id) {
                auto& e_map = std::get<0>(tuple);
                auto it_type = e_map.find(type);
                if (it_type != e_map.end())
                    it_type->second.remove_listener(id);
            }
        };

        template<typename... Ts>
        void unsubscribe_all(TEventEnum type, listener_id id) {
            const auto size = std::tuple_size<tuple_type>::value;
            unsubscribe_all_recurse<size - 1, Ts...>{}(_event_maps, type, id);
        }

        template<int index, typename... Ts>
        struct update_all_recurse {
            void operator() (tuple_type& tuple, float_seconds dt)
            {
                auto& e_map = std::get<index>(tuple);
                for (auto& pair : e_map)
                    pair.second.update(dt);

                update_all_recurse<index - 1, Ts...>{}(tuple, dt);
            }
        };

        template<typename... Ts>
        struct update_all_recurse<0, Ts...> {
            void operator() (tuple_type& tuple, float_seconds dt) {
                auto& e_map = std::get<0>(tuple);
                for (auto& pair : e_map)
                    pair.second.update(dt);

            }
        };

        template<typename... Ts>
        void update_all(event_maps& tuple, float_seconds dt) {
            const auto size = std::tuple_size<event_maps>::value;
            update_all_recurse<size - 1, Ts...>{}(tuple, dt);
        }
	};
}


#endif