
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <functional>
#include <thread>

#include <events/event_exchange.hpp>
#include <events/event.hpp>
#include <events/event_maps.hpp>

enum class app_event_type
{
	on_jump_key
};


typedef events::event_exchange<app_event_type,
	events::event<>,
	events::event<float>
> app_events;


class subscriber
{
public:
	subscriber(app_events& events) : _events(events)
	{
		l1 = events.subscribe(app_event_type::on_jump_key, []() {
			std::cout << "callback fires" << std::endl;
		});

		l2 = events.subscribe(
			app_event_type::on_jump_key, 
			std::bind(&subscriber::callback, this));
	}

	~subscriber()
	{
		_events.unsubscribe(app_event_type::on_jump_key, l1);
		_events.unsubscribe(app_event_type::on_jump_key, l2);
	}

private:
	app_events& _events;

	events::listener_id l1;
	events::listener_id l2;

	void callback()
	{
		std::cout << "callback fires2" << std::endl;
	}	
};

int main(int argc, char** argv)
{
	app_events events;
	subscriber sub(events);

	events.invoke(app_event_type::on_jump_key);

	return 0;
}