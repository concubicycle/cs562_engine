#ifndef WIZARD_UTIL_MATH_HPP
#define WIZARD_UTIL_MATH_HPP

namespace util
{
	template <typename T>
	T lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}
}

#endif