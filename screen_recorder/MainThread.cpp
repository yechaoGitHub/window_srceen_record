#include "MainThread.h"
#include <process.h>

extern unsigned __stdcall CaptrueThreadFunction(void *ptr_mian_thread_);

MainThread::MainThread():
	fps(0)
{

}

MainThread::~MainThread() 
{

}

void MainThread::Initial()
{
	fps = 60;
	quit = false;

	__source = new D3D11Source();
	__source->Initial();

	__encoder = new H264Encoder;
	__encoder->Initial();

	H264EncodeParameter par;
	par.width = 1920;
	par.height = 1080;
	par.bit_rate = 400000;
	par.time_base.num = 1;
	par.time_base.den = 60;
	par.gop_size = 10;
	par.max_b_frames = 0;
	par.pix_fmt = AV_PIX_FMT_NV12;

	__encoder->SetEncoderParameter(&par);

}

bool MainThread::Run()
{
	HANDLE th_ = (HANDLE)_beginthreadex(NULL, 0, CaptrueThreadFunction, this, 0, NULL);
	if (th_ == INVALID_HANDLE_VALUE)
		return false;

	ULONG_PTR mask = __GetFirstCpuMask();
	SetThreadAffinityMask(th_, mask);
	return true;
}

ULONG_PTR MainThread::__GetFirstCpuMask()
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