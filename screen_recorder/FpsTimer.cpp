#include "FpsTimer.h"
#include <process.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")

const uint64_t FpsTimer::_ONE_SECOND_IN_NS_(1000000000ULL);
LARGE_INTEGER FpsTimer::__clock_freq = { 0 };

extern GLFWwindow *_window_;

unsigned __stdcall  __YtimerThreadFunction(void *timer_instance) 
{
	FpsTimer *timer = reinterpret_cast<FpsTimer*>(timer_instance);
	uint64_t interval = FpsTimer::_ONE_SECOND_IN_NS_ / timer->__fps;
	uint64_t cur_time = FpsTimer::gettime_ns();
	uint64_t second_start = cur_time;
	uint64_t tager_time = cur_time + interval;
	uint64_t lag_count(0);
	uint64_t fps(0);
	uint64_t pass_second(0);

	glfwMakeContextCurrent(_window_);


	while (timer->__running) 
	{
		tager_time = cur_time + interval;

		//Ytimer 主循环回调
		(*timer->__call_back_loop)(timer->__user_data_loop, cur_time);

		fps++;
		if (FpsTimer::sleepto_ns(tager_time))
		{
			cur_time = tager_time;
		}
		else 
		{
			lag_count = (int)((FpsTimer::gettime_ns() - cur_time) / interval);
			cur_time = cur_time + interval * lag_count;
		}

		pass_second = cur_time - second_start;
		if (pass_second >= FpsTimer::_ONE_SECOND_IN_NS_)
		{
			second_start = cur_time;
			if (timer->__call_back_sec_pass) 
			{
				//Ytimer 每秒回调
				(*timer->__call_back_sec_pass)(timer->__user_data_loop, pass_second, fps, cur_time);
			}
			pass_second = 0;
			fps = 0;
		}
	}

	return 0;
}

FpsTimer::FpsTimer() :
	__user_data_loop(nullptr),
	__user_data_sec_pass(nullptr),
	__call_back_loop(nullptr),
	__call_back_sec_pass(nullptr),
	__running(false),
	__fps(0),
	__th(INVALID_HANDLE_VALUE)
{

}

FpsTimer::~FpsTimer()
{

}


bool FpsTimer::SetLoop(YtimerLoopFunction call_back_func_, uint32_t fps_, void *user_data_)
{
	if (call_back_func_ == nullptr || fps_ == 0)
		return false;

	__call_back_loop = call_back_func_;
	__user_data_loop = user_data_;
	__fps = fps_;

	return true;
}

bool FpsTimer::SetSecondPassCallBack(YtimerSecondPassFunction call_back_func_, void *user_data_)
{
	if (call_back_func_ == nullptr)
		return false;

	__call_back_sec_pass = call_back_func_;
	__user_data_sec_pass = user_data_;

	return true;
}

bool FpsTimer::RunLoop()
{
	__th = (HANDLE)_beginthreadex(NULL, 0, __YtimerThreadFunction, this, 0, NULL);
	if (__th == INVALID_HANDLE_VALUE)
		return false;

	__running = true;
	//设置cpu亲和性
	ULONG_PTR mask = __GetFirstCpuMask();
	SetThreadAffinityMask(__th, mask);
	
	return true;
}

bool FpsTimer::IsRunning()
{
	if (__running == true && __th != INVALID_HANDLE_VALUE)
		return true;
	else 
		return false;
}

void FpsTimer::Stop()
{
	__running = false;
}

bool FpsTimer::WaitToStop(int64_t time_wait)
{
	__running = false;
	DWORD ret = WaitForSingleObject(__th, time_wait);
	if (ret == WAIT_OBJECT_0) 
	{
		__th = INVALID_HANDLE_VALUE;
	}
	else 
	{
		return false;
	}
}

ULONG_PTR FpsTimer::__GetFirstCpuMask()
{
	ULONG_PTR  sys_mask_(0), proc_mask_(0), cpu_mask_(0), compare_(1);

	bool ret_ = ::GetProcessAffinityMask(::GetCurrentProcess(), &sys_mask_, &proc_mask_);
	if (ret_ == 0)
		return -1;

	for (int i = 0; i < sizeof(ULONG_PTR) * 8; i++)
	{
		if ((proc_mask_ & compare_) != 0)
		{
			return compare_;
		}
		else
		{
			compare_ >= 1;
		}
	}

	return -1;
}