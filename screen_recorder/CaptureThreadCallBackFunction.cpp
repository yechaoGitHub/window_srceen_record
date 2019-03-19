#include "MainThread.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "RgbToNv12.cpp"

//#include "assert.h"
//#include "Opengl.h"
//#include "OutputYuv.h"
//#include "RgbToNv12.cpp"
//
//#pragma comment(lib, "glfw3.lib")

void CaptrueThreadFunction(void *user_data, uint64_t cur_time)
{
	MainThread *thread = reinterpret_cast<MainThread*>(user_data);
	D3D11Source &d3d_src = *thread->__source;
	OpenglConvertRgbToNV12 &convert = thread->__convert;
	H264Encoder	&encoder = *thread->__encoder;

	static uint8_t *data[4] = { 0 };
	static int line_size[4] = { 0 };
	static uint8_t *nv12_data[4] = { 0 };
	static int nv12_line_size[4] = { 0 };
	static void *_data_ = NULL;
	static int size = 0;

	static int yuv_len = (1920 * 1080 * 3) / 2;
	static unsigned char *buff = nullptr;
	if (buff == nullptr) 
	{
		buff = new unsigned char[yuv_len];
	}

	if (d3d_src.Capture()) 
	{
		d3d_src.GetData(data, line_size);
		convert.ConvertRgbToNV12(data[0], 1920, 1080, buff);
		nv12_data[0] = buff;
		nv12_line_size[0] = 1920;
		nv12_data[1] = buff + 1920 * 1080;
		nv12_line_size[1] = 1920;

		encoder.SetOriginData(nv12_data, nv12_line_size);
		encoder.Encode();
		encoder.GetEncodeData(_data_, &size);

		encoder.ReleaseEncodeData();
		d3d_src.ReleaseData();
	}
}

void ShowFps(void *user_data, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time) 
{
	std::cout << fps << "\n";
}

//unsigned __stdcall CaptrueThreadFunction(void *ptr_mian_thread_) 
//{
//	MainThread *thread =  reinterpret_cast<MainThread*>(ptr_mian_thread_);
//	assert(thread != NULL);
//
//	glfwInit();
//	GLFWwindow* window = glfwCreateWindow(100, 100, "LearnOpenGL", nullptr, nullptr);
//
//	glfwMakeContextCurrent(window);
//	OpenglConvertRgbToNV12 conver;
//	conver.Initial();
//
//	timer.Initial();
//
//	uint64_t interval = Ytimer::_ONE_SECOND_IN_NS_ / thread->fps;
//	thread->main_time = timer.gettime_ns();
//	uint64_t tager_time = thread->main_time + interval;
//	uint64_t cur_time = thread->main_time;
//	uint64_t count(0);
//
//	uint8_t *data[4] = { 0 };
//	int line_size[4] = { 0 };
//	void *_data_ = NULL;
//	int size = 0;
//
//	uint8_t *nv12_data[4] = { 0 };
//	int nv12_line_size[4] = { 0 };
//
//	int yuv_len = (1920 * 1080 * 3) / 2;
//	unsigned char *buff = new unsigned char[yuv_len];
//	thread->__encoder->StartRecording("./my.mp4");
//
//	OutputYuv oyuv;
//	oyuv.Initial("./test.yuv");
//
//	int fps_count(0);
//	while (!thread->quit)
//	{
//		cur_time = thread->main_time;
//
//		/*to do*/
//		//if (thread->__source->Capture()) 
//		//{
//		//	thread->__source->GetData(data, line_size);
//
//		//	//int i = clock();
//		//	conver.ConvertRgbToNV12(data[0], 1920, 1080, buff);
//		//	Rgb2NV12(data[0], 4, 1920, 1080, buff);
//		//	oyuv.WriteYuv(buff, yuv_len);
//		//	
//		//	//int b = clock() - i;
//
//		//	//nv12_data[0] = buff;
//		//	//nv12_line_size[0] = 1920;
//		//	//nv12_data[1] = buff + 1920 * 1080;
//		//	//nv12_line_size[1] = 1920;
//
//		//	//thread->__encoder->SetOriginData(nv12_data, nv12_line_size);
//		//	//thread->__encoder->Encode();
//		//	//thread->__encoder->GetEncodeData(_data_, &size);
//
//		//	//thread->__encoder->ReleaseEncodeData();
//		//	thread->__source->ReleaseData();
//		//}
//
//
//		if (timer.sleepto_ns(tager_time)) 
//		{
//			thread->main_time = tager_time;
//		}
//		else 
//		{
//			count = (int)((timer.gettime_ns() - cur_time) / interval);
//			thread->main_time = cur_time + interval * count;
//		}
//
//	}
//	//thread->__encoder->EndRecording();
//	//oyuv.close();
//
//	return 0;
//
//}