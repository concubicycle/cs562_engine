#ifndef __FPS_DISPLAY_HPP_
#define __FPS_DISPLAY_HPP_

#include <core/frame_timer.hpp>
#include <engine-ui/view.hpp>

namespace engineui
{

	class fps_display : public view
	{
	public:
		fps_display(core::viewport& viewport, core::frame_timer& timer);

		void draw();

	private:
		core::frame_timer& _frame_timer;
	};

}

#endif