#pragma once
#include <windows.h>
#include <stdint.h>
#include "D3D11Source.h"
#include "H264Encoder.h"

class MainThread 
{
	friend unsigned __stdcall CaptrueThreadFunction(void *ptr_mian_thread_);

public:
	MainThread();
	~MainThread();

	uint64_t fps;
	uint64_t main_time;
	bool quit;


	void Initial();

	bool Run();

private:
	ULONG_PTR	__cpu_mask;

	static ULONG_PTR __GetFirstCpuMask();

	D3D11Source *__source;
	H264Encoder *__encoder;

	//std::list<>

};
