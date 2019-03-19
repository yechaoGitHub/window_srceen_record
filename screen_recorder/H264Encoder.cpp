#include "H264Encoder.h"
#include "assert.h"

H264Encoder::H264Encoder() :
	__codec_id(AV_CODEC_ID_H264),
	__codec(NULL),
	__codec_ctx(NULL),
	__fmt_ctx(NULL),
	__frame(NULL),
	__pkt(NULL),
	__recording(NULL),
	__ofmt_ctx(NULL),
	__video_st(NULL),
	__pts_count(0)
{

}

H264Encoder::~H264Encoder() 
{

}

bool H264Encoder::Initial() 
{
	int ret = 0;

	__codec = avcodec_find_encoder(__codec_id);
	if (__codec == NULL)
		goto error;

	__codec_ctx = avcodec_alloc_context3(__codec);
	if (__codec_ctx == NULL)
		goto error;

	__frame = av_frame_alloc();
	if (__frame == NULL)
		goto error;

	__pkt = av_packet_alloc();
	if (__pkt == NULL)
		goto error;
	
	av_init_packet(__pkt);

error:
	return false;
}

bool H264Encoder::SetEncoderParameter(const H264EncodeParameter *parameter)
{
	int ret(0);

	if (__codec_ctx == NULL)
		goto error;

	__codec_ctx->width = parameter->width;
	__codec_ctx->height = parameter->height;
	__codec_ctx->time_base = parameter->time_base;
	__codec_ctx->gop_size = parameter->gop_size;
	__codec_ctx->max_b_frames = parameter->max_b_frames;
	__codec_ctx->bit_rate = parameter->bit_rate;
	__codec_ctx->pix_fmt = (AVPixelFormat)parameter->pix_fmt;

	av_opt_set(__codec_ctx->priv_data, "preset", "ultrafast", 0);
	av_opt_set(__codec_ctx->priv_data, "tune", "zerolatency", 0);

	ret = avcodec_open2(__codec_ctx, __codec, NULL);
	if (ret < 0)
		goto error;

	__frame->format = __codec_ctx->pix_fmt;
	__frame->width	= __codec_ctx->width;
	__frame->height = __codec_ctx->height;

	__paramter = *parameter;

	return true;

error:
	return false;
}

bool H264Encoder::SetOriginData(uint8_t *data[4], uint32_t line_size[4])
{
	if (__frame == NULL)
		goto error;

	memcpy(__frame->data, data, sizeof(uint8_t*) * 4);
	memcpy(__frame->linesize, line_size, sizeof(int) * 4);

	return true;
error:
	return false;
}

bool H264Encoder::Encode()
{
	if (__frame == NULL || __codec_ctx == NULL)
		return false;

	if (__ofmt_ctx != NULL)
		__frame->pts = av_rescale_q(__pts_count, __codec_ctx->time_base, __video_st->time_base);

	int ret = avcodec_send_frame(__codec_ctx, __frame);
	if (ret < 0)
		goto error;
	
	if (__pkt->data != NULL) 
	{
		av_packet_unref(__pkt);
	}

	ret = avcodec_receive_packet(__codec_ctx, __pkt);
	if (ret < 0)
		goto error;

	if (__ofmt_ctx != NULL) 
	{
		__pkt->stream_index = __video_st->index;
		ret = av_interleaved_write_frame(__ofmt_ctx, __pkt);
		if (ret < 0) 
		{
			int i = 0;
			i++;
		}
		__pts_count++;
	}


	return true;
error:
	return false;
}

bool H264Encoder::GetEncodeData(void *data, int *size)
{
	if (__pkt == NULL)
		goto error;

	data	= __pkt->data;
	*size	= __pkt->size;

	return true;

error:
	return false;
}

bool H264Encoder::ReleaseEncodeData()
{
	if (__pkt == NULL)
		goto error;

	av_packet_unref(__pkt);

	return true;

error:
	return false;
}

bool H264Encoder::StartRecording(const char *file_path)
{
	if (file_path == NULL)
		return false;

	if (__ofmt_ctx != NULL) 
	{
		EndRecording();
	}

	__out_file = file_path;
	__pts_count = 0;

	int ret(0);
	ret = avformat_alloc_output_context2(&__ofmt_ctx, NULL, NULL, file_path);
	if (ret < 0) 
	{
		goto error;
	}

	ret = avio_open(&__ofmt_ctx->pb, file_path, AVIO_FLAG_READ_WRITE);
	if (ret < 0)
		goto error;

	__video_st = avformat_new_stream(__ofmt_ctx, 0);
	if (__video_st == NULL)
		goto error;

	ret = avcodec_parameters_from_context(__video_st->codecpar, __codec_ctx);
	if (ret < 0)
		goto error;

	__video_st->time_base = __paramter.time_base;

	avformat_write_header(__ofmt_ctx, NULL);

error:
	return false;
}

bool H264Encoder::EndRecording()
{
	av_write_trailer(__ofmt_ctx);
	avio_close(__ofmt_ctx->pb);
	avformat_free_context(__ofmt_ctx);
	__ofmt_ctx = NULL;

	return true;
}
