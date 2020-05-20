#ifndef __SIGN_HPP_
#define __SIGN_HPP_

namespace util
{
	template<typename T>
	int sign(T val)
	{
		return val >= 0 ? 1 : -1;
	}
}

#endif