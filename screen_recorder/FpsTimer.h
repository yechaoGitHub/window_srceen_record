#pragma once
#include <stdint.h>
#include <windows.h>

class FpsTimer 
{
public:
	FpsTimer();
	~FpsTimer();
	
	typedef void(__stdcall *YtimerLoopFunction)(void *user_data_, uint64_t cur_time);
	typedef void(__stdcall *YtimerSecondPassFunction)(void *user_data_, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time);

	static const uint64_t _ONE_SECOND_IN_NS_;

	void Initial() 
	{
		__clock_freq.QuadPart = get_clockfreq();
	}

	static inline uint64_t gettime_ns() 
	{
		LARGE_INTEGER current_time;
		double time_val;

		QueryPerformanceCounter(&current_time);
		time_val = (double)current_time.QuadPart;
		time_val *= 1000000000.0;
		time_val /= __clock_freq.QuadPart;

		return (uint64_t)time_val;
	}

	static inline bool sleepto_ns(uint64_t time_target)
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

	static inline void sleep_ns(uint64_t time_ns)
	{
		uint64_t t = gettime_ns();
		uint32_t milliseconds(0);
		uint32_t ns_remainder(0);
		milliseconds = (uint32_t)(time_ns / 1000000);
		ns_remainder = (uint32_t)(time_ns % 1000000);

		if (milliseconds > 1)
			Sleep(milliseconds - 1);

		for (;;)
		{
			if ((gettime_ns() - t) >= time_ns) 
			{
				Sleep(0);
			}
			else 
			{
				return;
			}
			
		}
	}

	//设置主循环回调函数
	bool SetLoop(YtimerLoopFunction call_back_func_, uint32_t fps_, void *user_data_ = nullptr);
	//设置每过一秒回调一次的回调函数
	bool SetSecondPassCallBack(YtimerSecondPassFunction call_back_func_, void *user_data_ = nullptr);
	bool RunLoop();
	bool IsRunning();
	void Stop();
	bool WaitToStop(int64_t time_wait = -1);

private:
	static LARGE_INTEGER		__clock_freq;
	void						*__user_data_loop;
	void						*__user_data_sec_pass;
	YtimerLoopFunction			__call_back_loop;
	YtimerSecondPassFunction	__call_back_sec_pass;
	bool						__running;
	uint32_t					__fps;
	HANDLE						__th;

	//获取cpu频率
	static inline uint64_t get_clockfreq()
	{
		QueryPerformanceFrequency(&__clock_freq);

		return __clock_freq.QuadPart;
	}

	//Ytimer 主循环函数
	friend unsigned __stdcall __YtimerThreadFunction(void *timer_instance);
	//获取第一个可用cpu掩码
	static ULONG_PTR __GetFirstCpuMask();
};
