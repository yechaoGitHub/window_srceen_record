#pragma once

//������һ��step��RGBΪ3��RGBAΪ4��
void Rgb2NV12(const unsigned char I[], int step,
	const int image_width,
	const int image_height,
	unsigned char J[]);
