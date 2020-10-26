#ifndef __ANIMATION_TIME_HPP_
#define __ANIMATION_TIME_HPP_

#include <chrono>

namespace renderer
{
	typedef std::chrono::duration<double> animation_time;
	typedef std::chrono::time_point<std::chrono::system_clock, animation_time> animation_time_point;
}


#endif