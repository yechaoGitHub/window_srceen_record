#pragma once
#include "EncodeParameter.h"
#include <string>

extern "C"
{
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
}

#pragma comment (lib, "libavcodecd.lib")
#pragma comment (lib, "libavformatd.lib")

struct H264EncodeParameter : public EncodeParameter
{
	H264EncodeParameter() 
	{
		gop_size = 0;
		max_b_frames = 0;
	}

	AVRational	time_base;
	int			gop_size;
	int			max_b_frames;
};


class H264Encoder 
{
public:
	H264Encoder();
	~H264Encoder();

	bool Initial();
	bool SetEncoderParameter(const H264EncodeParameter *parameter);
	bool SetOriginData(uint8_t *data[4], int line_size[4]);
	bool Encode();
	bool GetEncodeData(void *data, int *size);
	bool ReleaseEncodeData();
	bool StartRecording(const char *file_path);
	bool EndRecording();

private:
	H264EncodeParameter __paramter;
	const AVCodecID		__codec_id;
	AVCodec				*__codec;
	AVCodecContext		*__codec_ctx;
	AVFormatContext		*__fmt_ctx;
	AVStream			*__video_stream;
	AVFrame				*__frame;
	AVPacket			*__pkt;
	AVFormatContext		*__ofmt_ctx;
	AVStream			*__video_st;
	bool				__recording;
	std::string			__out_file;
	uint64_t			__pts_count;
};