#ifndef __SLEEP_H_
#define __SLEEP_H_

#include <chrono>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#endif


namespace os
{
	class sleeper
	{
		static const std::chrono::seconds DefaultTimeout;

	public:
		sleeper(std::chrono::seconds timeout = DefaultTimeout);
		~sleeper();

		void sleep(std::chrono::nanoseconds nanoseconds);

	private:
		std::chrono::seconds _timeout;

#ifdef _WIN32
		TIMECAPS tc;
		UINT wTimerRes;
#endif
	};

} // namespace os

#endif
