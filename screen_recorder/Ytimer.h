#pragma once
#include <stdint.h>
#include <windows.h>

class Ytimer 
{
public:
	Ytimer() : __clock_freq({ 0 }) {}
	~Ytimer(){}

	static uint64_t _ONE_SECOND_IN_NS_;

	void Initial() 
	{
		__clock_freq.QuadPart = get_clockfreq();
	}

	inline uint64_t gettime_ns() 
	{
		LARGE_INTEGER current_time;
		double time_val;

		QueryPerformanceCounter(&current_time);
		time_val = (double)current_time.QuadPart;
		time_val *= 1000000000.0;
		time_val /= __clock_freq.QuadPart;

		return (uint64_t)time_val;
	}

	inline bool sleepto_ns(uint64_t time_target)
	{
		uint64_t t = gettime_ns();
		uint32_t milliseconds;

		if (t >= time_target)
			return false;

		milliseconds = (uint32_t)((time_target - t) / 1000000);
		if (milliseconds > 1)
			Sleep(milliseconds - 1);

		for (;;) {
			t = gettime_ns();
			if (t >= time_target)
				return true;

			Sleep(0);

		}
	}

	inline uint64_t get_clockfreq()
	{
		QueryPerformanceFrequency(&__clock_freq);

		return __clock_freq.QuadPart;
	}

private:
	LARGE_INTEGER __clock_freq;
};
