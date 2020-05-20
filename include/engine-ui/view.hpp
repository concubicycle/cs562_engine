#ifndef __ENGINE_VIEW_HPP_
#define __ENGINE_VIEW_HPP_

#include <core/viewport.hpp>

namespace engineui
{

	class view
	{
	public:
		view(core::viewport& viewport) : _viewport(viewport) {}
		~view() {}
		virtual void draw() = 0;
		virtual void on_show() {}

	protected:
		core::viewport& _viewport;
	};

}

#endif