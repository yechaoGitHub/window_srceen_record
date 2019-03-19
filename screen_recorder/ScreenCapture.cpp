#include "ScreenCapture.h"
#include <process.h>

extern void __stdcall CaptrueThreadFunction(void *user_data, uint64_t cur_time);
extern void __stdcall ShowFps(void *user_data, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time);

ScreenCapture::ScreenCapture()
{

}

ScreenCapture::~ScreenCapture() 
{

}

void ScreenCapture::Initial()
{
	__source = new D3D11Source();
	__source->Initial();

	__encoder = new H264Encoder;
	__encoder->Initial();

	H264EncodeParameter par;
	par.width = 1920;
	par.height = 1080;
	par.bit_rate = 4000000;
	par.time_base.num = 1;
	par.time_base.den = 25;
	par.gop_size = 10;
	par.max_b_frames = 0;
	par.pix_fmt = AV_PIX_FMT_NV21;

	__encoder->SetEncoderParameter(&par);

	__timer.Initial();
	__timer.SetLoop(CaptrueThreadFunction, 25, this);
	__timer.SetSecondPassCallBack(ShowFps, this);

	__convert.Initial();

}

bool ScreenCapture::Run()
{
	__timer.RunLoop();
	return true;
}

