// screen_recorder.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "assert.h"
#include "MainThread.h"

int main()
{
	MainThread mth;

	mth.Initial();

	mth.Run();

	while (1);

	return 0;
}

