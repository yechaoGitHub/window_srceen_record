#include "Ytimer.h"
#include "MainThread.h"
#include "assert.h"
#include "RgbToNv12.cpp"
#include "Opengl.h"
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")
Ytimer timer;

unsigned __stdcall CaptrueThreadFunction(void *ptr_mian_thread_) 
{
	MainThread *thread =  reinterpret_cast<MainThread*>(ptr_mian_thread_);
	assert(thread != NULL);

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(100, 100, "LearnOpenGL", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	OpenglConvertRgbToNV12 conver;
	conver.Initial();

	timer.Initial();

	uint64_t interval = Ytimer::_ONE_SECOND_IN_NS_ / thread->fps;
	thread->main_time = timer.gettime_ns();
	uint64_t tager_time = thread->main_time + interval;
	uint64_t cur_time = thread->main_time;
	uint64_t count = 0;

	uint8_t *data[4] = { 0 };
	int line_size[4] = { 0 };
	void *_data_ = NULL;
	int size = 0;

	uint8_t *nv12_data[4] = { 0 };
	int nv12_line_size[4] = { 0 };

	unsigned char *buff = new unsigned char[(1920 * 1080 * 3) / 2];
	thread->__encoder->StartRecording("./my.mp4");
	while (!thread->quit)
	{
		cur_time = thread->main_time;

		/*to do*/
		if (thread->__source->Capture()) 
		{
			thread->__source->GetData(data, line_size);

			int i = clock();
			conver.ConvertRgbToNV12(data[0], 1920, 1080, buff);
			int b = clock() - i;

			nv12_data[0] = buff;
			nv12_line_size[0] = 1920;
			nv12_data[1] = buff + 1920 * 1080;
			nv12_line_size[1] = 1920;

			thread->__encoder->SetOriginData(nv12_data, nv12_line_size);
			thread->__encoder->Encode();
			thread->__encoder->GetEncodeData(_data_, &size);

			thread->__encoder->ReleaseEncodeData();
			thread->__source->ReleaseData();
		}

		if (timer.sleepto_ns(tager_time)) 
		{
			thread->main_time = tager_time;
		}
		else 
		{
			count = (int)((timer.gettime_ns() - cur_time) / interval);
			thread->main_time = cur_time + interval * count;
		}

	}
	thread->__encoder->EndRecording();

	return 0;

}