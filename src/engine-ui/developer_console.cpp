#include <engine-ui/developer_console.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


int input_callback(ImGuiInputTextCallbackData* data)
{
	bool is_filter_callback = data->Flags & ImGuiInputTextFlags_CallbackCharFilter;
	if (is_filter_callback)
	{
		if (data->EventChar == '`')
		{
			return 1;
		}
	}
	return 0;
}

engineui::developer_console::developer_console(core::viewport& viewport, event::EventManager& events, GLFWwindow* window)
	: view(viewport)
	, _events(events)
	, _window(window)
{
	setvbuf(stdout, _outbuf, _IOFBF, BUFSIZ);
	setvbuf(stderr, _errbuf, _IOFBF, BUFSIZ);
}

void engineui::developer_console::draw()
{
	ImGui::SetNextWindowSize({ (float)_viewport.width, Height }, ImGuiCond_Always);
	ImGui::Begin("DevConsole");

	if (ImGui::IsWindowCollapsed())
		ImGui::SetWindowPos({ 0, (float)_viewport.height - ImGui::GetWindowHeight() }, ImGuiCond_Always);
	else
		ImGui::SetWindowPos({ 0, (float)_viewport.height - Height }, ImGuiCond_Always);

	static int output_type = 0;
#ifndef NDEBUG
	ImGui::RadioButton("command", &output_type, 0); ImGui::SameLine();
	ImGui::RadioButton("stdout", &output_type, 1); ImGui::SameLine();
	ImGui::RadioButton("stderr", &output_type, 2);
#endif

	ImGui::PushItemWidth(-1);

	switch (output_type)
	{
	case 0:
	{
		ImGui::InputTextMultiline("##cmd", _command, IM_ARRAYSIZE(_command), { 0, Height - 85 }, ImGuiInputTextFlags_ReadOnly);

		if (_should_grab_focus)
		{
			ImGui::SetKeyboardFocusHere();
			_should_grab_focus = false;
		}

		auto pressed = ImGui::InputText("##in", _input, IM_ARRAYSIZE(_input) - 1, 
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, &input_callback);
		if (pressed) handle_command();
		break;
	}
#ifndef NDEBUG
	case 1:
		write_buffer(_output, _outbuf);
		ImGui::InputTextMultiline("##out", _output, IM_ARRAYSIZE(_output), { 0, Height - 60 }, ImGuiInputTextFlags_ReadOnly);
		break;
	case 2:
		write_buffer(_error, _errbuf);
		ImGui::InputTextMultiline("##err", _error, IM_ARRAYSIZE(_error), { 0, Height - 60 }, ImGuiInputTextFlags_ReadOnly);
		break;
#endif
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void engineui::developer_console::handle_command()
{
	if (strcmp(_input, "exit") == 0)
		glfwSetWindowShouldClose(_window, true);

	if (strcmp(_input, "noclip") == 0)
	{
		noclip triggeredEvent;
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "toggle_P1_AI") == 0)
	{
		cheat triggeredEvent("toggle_P1_AI");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "toggle_P2_AI") == 0)
	{
		cheat triggeredEvent("toggle_P2_AI");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "attract_mode") == 0)
	{
		cheat triggeredEvent("attract_mode");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "refill_energy") == 0)
	{
		cheat triggeredEvent("refill_energy");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "P1_wins") == 0)
	{
		cheat triggeredEvent("P1_wins");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "P2_wins") == 0)
	{
		cheat triggeredEvent("P2_wins");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "P1_almost_wins") == 0)
	{
		cheat triggeredEvent("P1_almost_wins");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "P2_almost_wins") == 0)
	{
		cheat triggeredEvent("P2_almost_wins");
		_events.BroadcastEvent(triggeredEvent);
	}
	if (strcmp(_input, "redraw_hands") == 0)
	{
		cheat triggeredEvent("redraw_hands");
		_events.BroadcastEvent(triggeredEvent);
	}

	write_buffer(_command, "> ");
	write_buffer(_command, _input);
	write_buffer(_command, "\n");
	_input[0] = '\0';
}

void engineui::developer_console::write_buffer(char* output, char* buffer)
{
	auto buflen = strlen(buffer);
	auto outlen = strlen(output);

	if (outlen + buflen > BufSize)
	{
		auto overflow = (outlen + buflen) - BufSize + 1;
		auto writelen = outlen - overflow;
		memcpy(output, output + overflow, writelen);
		outlen -= overflow;
	}

	memcpy(output + outlen, buffer, buflen);
}

void engineui::developer_console::on_show()
{
	_should_grab_focus = true;
	_input[0] = '\0';
}

char engineui::developer_console::_outbuf[BUFSIZ]{ "" };
char engineui::developer_console::_errbuf[BUFSIZ]{ "" };
