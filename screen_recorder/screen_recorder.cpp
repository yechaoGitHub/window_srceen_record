// screen_recorder.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "assert.h"
#include "ScreenCapture.h"
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")

GLFWwindow *_window_ = nullptr;

int main()
{
	glfwInit();
	_window_ = glfwCreateWindow(10, 10, "aa", nullptr, nullptr);
	glfwMakeContextCurrent(_window_);

	ScreenCapture mth;

	mth.Initial();
	glfwMakeContextCurrent(NULL);
	mth.__encoder.StartRecording("./t.mp4");
	mth.Run();

	getchar();

	mth.__timer.WaitToStop();
	mth.__encoder.EndRecording();
	return 0;
}

