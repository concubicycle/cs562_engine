#ifndef __CONSOLE_COMMAND_EVENT_HPP_
#define __CONSOLE_COMMAND_EVENT_HPP_

#include <string>

#include <event/event.hpp>

namespace core
{
	class console_command_event : public Event;
	{
	public:
		console_command_event(std::string c) : command(c) {}	
		const std::string command;
	};
}



#endif