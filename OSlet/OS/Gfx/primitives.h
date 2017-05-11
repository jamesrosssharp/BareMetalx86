
#pragma once

enum PixelFormat
{

	PIXELFORMAT_INDEXED8	= 	0,
	PIXELFORMAT_RGB16,
	PIXELFORMAT_RGB24,
	PIXELFORMAT_RGB32

};

struct RGBColor
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

struct Image
{
	unsigned int width;
	unsigned int height;
	void*	data;
	enum PixelFormat pixelFormat;
};

unsigned int gfx_prim_rgbColTo32(struct RGBColor* col);

