#pragma once

//增加了一个step，RGB为3，RGBA为4。
void Rgb2NV12(const unsigned char I[], int step,
	const int image_width,
	const int image_height,
	unsigned char J[]);
