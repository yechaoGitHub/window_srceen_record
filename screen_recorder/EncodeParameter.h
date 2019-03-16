#pragma once
#include <stdint.h>

struct EncodeParameter
{
	EncodeParameter() 
	{
		width = 0;
		height = 0;
		bit_rate = 0;
		pix_fmt = 0;
	}

	uint32_t width;
	uint32_t height;
	uint32_t bit_rate;
	uint32_t pix_fmt;
};