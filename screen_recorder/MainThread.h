#pragma once
#include <windows.h>
#include <stdint.h>
#include "FpsTimer.h"
#include "D3D11Source.h"
#include "H264Encoder.h"
#include "Opengl.h"

class MainThread 
{
public:
	MainThread();
	~MainThread();

	void Initial();

	bool Run();

private:
	FpsTimer				__timer;
	D3D11Source				*__source;
	H264Encoder				*__encoder;
	OpenglConvertRgbToNV12	__convert;

	friend void __stdcall CaptrueThreadFunction(void *user_data, uint64_t cur_time);
	friend void __stdcall ShowFps(void *user_data, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time);
};
