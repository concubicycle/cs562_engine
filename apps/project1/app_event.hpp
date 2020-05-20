#ifndef __APP_EVENTS_
#define __APP_EVENTS_

#include <events/event.hpp>
#include <events/event_exchange.hpp>

enum class app_event_type
{
	on_jump_key
};


typedef events::event_exchange<app_event_type,
	events::event<>,
	events::event<float>
> app_events;


#endif