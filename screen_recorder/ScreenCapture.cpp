#include "ScreenCapture.h"
#include <process.h>

extern void __stdcall CaptrueThreadFunction(void *user_data, uint64_t cur_time);
extern void __stdcall ShowFps(void *user_data, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time);

ScreenCapture::ScreenCapture():
	__nv12_buffer(nullptr),
	__nv12_line_data{ 0 },
	__nv12_line_size{ 0 },
	__screen_width(0),
	__screen_height(0)
{

}

ScreenCapture::~ScreenCapture() 
{

}

bool ScreenCapture::Initial()
{
	bool ret(false);

	ret = __source.Initial();
	if (!ret) return false;
	
	__encoder.Initial();
	if (!ret) return false;

	
	ret = __source.GetScreenSize(__screen_width, __screen_height);
	if (!ret) return false;

	H264EncodeParameter par;
	par.width = __screen_width;
	par.height = __screen_height;
	par.bit_rate = 4000000;
	par.time_base.num = 1;
	par.time_base.den = 25;
	par.gop_size = 10;
	par.max_b_frames = 0;
	par.pix_fmt = AV_PIX_FMT_NV21;
	ret = __encoder.SetEncoderParameter(&par);
	if (!ret) return false;

	__timer.Initial();
	ret = __timer.SetLoop(CaptrueThreadFunction, 25, this);
	if (!ret) return false;

	ret = __timer.SetSecondPassCallBack(ShowFps, this);
	if (!ret) return false;

	ret = __convert.Initial();
	if (!ret) return false;

	__nv12_buffer = __AllocConvertBuffer(__screen_width, __screen_height);
	if (__nv12_buffer == nullptr) return false;

	return true;
}

bool ScreenCapture::Run()
{
	__timer.RunLoop();
	return true;
}

void* ScreenCapture::__AllocConvertBuffer(int width, int height)
{
	return new char[width * height * 4];
}

void ScreenCapture::__RecordTimeStart(RecordStage stage)
{
	static uint64_t cur_time(0);
	cur_time = FpsTimer::gettime_ns();

	switch (stage)
	{
	case ScreenCapture::GET_SRC_DATA:
		__time_src_data = cur_time;
	break;

	case ScreenCapture::CONVERT_TO_NV12:
		__time_convert = cur_time;
	break;

	case ScreenCapture::ENCODE:
		__time_encode = cur_time;
	break;

	case ScreenCapture::OUTPUT_FILE:
		__time_outfile = cur_time;
	break;

	default:
	break;
	}
}

void ScreenCapture::__RecordTimeEnd(RecordStage stage)
{
	static uint64_t cur_time(0);
	cur_time = FpsTimer::gettime_ns();

	switch (stage)
	{
	case ScreenCapture::GET_SRC_DATA:
		__time_src_data = cur_time - __time_src_data;
	break;

	case ScreenCapture::CONVERT_TO_NV12:
		__time_convert = cur_time - __time_convert;
	break;

	case ScreenCapture::ENCODE:
		__time_encode = cur_time - __time_encode;
	break;

	case ScreenCapture::OUTPUT_FILE:
		__time_outfile = cur_time - __time_outfile;
	break;

	default:
	break;
	}
}