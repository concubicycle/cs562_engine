#include <engine-ui/imgui_overlay.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::imgui_overlay::imgui_overlay(GLFWwindow* window, core::game_input_manager& input, core::cursor_state& cursor)	
	:_input(input)
	, _cursor(cursor)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void engineui::imgui_overlay::update()
{
	bool visible_next = _visible;
	if (_input.is_input_started(core::controls::DEVELOPER_CONSOLE))
	{
		visible_next = !_visible;

		if (visible_next)
		{
			_cursor.push_enable();
			for (auto* v : _views) v->on_show();
		}
		else _cursor.pop();
	}

	if (_visible && _visible == visible_next)
		render_views();

	_visible = visible_next;
}

void engineui::imgui_overlay::render_views()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	// Draw windows
	for (auto* v : _views) v->draw();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}