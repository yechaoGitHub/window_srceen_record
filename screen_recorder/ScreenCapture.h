#pragma once
#include <windows.h>
#include <stdint.h>
#include "FpsTimer.h"
#include "D3D11Source.h"
#include "H264Encoder.h"
#include "Opengl.h"

class ScreenCapture 
{
	friend void __stdcall CaptrueThreadFunction(void *user_data, uint64_t cur_time);
	friend void __stdcall ShowFps(void *user_data, uint64_t pass_second_ns, uint64_t fps, uint64_t cur_time);

public:
	ScreenCapture();
	~ScreenCapture();

	bool Initial();

	bool Run();

public:
	enum RecordStage {GET_SRC_DATA, CONVERT_TO_NV12, ENCODE, OUTPUT_FILE};

	FpsTimer				__timer;
	D3D11Source				__source;
	H264Encoder				__encoder;
	OpenglConvertRgbToNV12	__convert;
	void					*__nv12_buffer;
	uint8_t					*__nv12_line_data[4];
	uint32_t				__nv12_line_size[4];
	uint32_t				__screen_width;
	uint32_t				__screen_height;
	
	uint64_t				__time_src_data;
	uint64_t				__time_convert;
	uint64_t				__time_encode;
	uint64_t				__time_outfile;

	void* __AllocConvertBuffer(int width, int height);
	void __RecordTimeStart(RecordStage stage);
	void __RecordTimeEnd(RecordStage stage);
};
