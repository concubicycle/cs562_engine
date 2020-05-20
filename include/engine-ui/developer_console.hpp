#ifndef __DEVCONSOLE_HPP_
#define __DEVCONSOLE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class noclip : public event::Event
{
public:
	noclip() : Event(event::EVENT_TYPE::TOGGLE_NOCLIP) {}
};
class cheat : public event::Event
{
public:
	cheat(std::string data) : Event(event::EVENT_TYPE::CHEAT, data) {}
};

namespace engineui
{

	class developer_console : public view
	{
		static const std::uint16_t BufSize = 4096;
		static const std::uint32_t Height = 256;

	public:
		developer_console(core::viewport& viewport, event::EventManager& events, GLFWwindow* window);
		void draw();
		void on_show() override;

	private:
		// keep these static
		static char _outbuf[BUFSIZ];
		static char _errbuf[BUFSIZ];

		event::EventManager& _events;
		GLFWwindow* _window;

		char _command[BufSize]{ "" };
		char _output[BufSize]{ "" };
		char _error[BufSize]{ "" };
		char _input[BufSize]{ "" };

		bool _should_grab_focus{ true };

		void handle_command();

		void write_buffer(char* output, char* buffer);
	};
	
}

#endif
