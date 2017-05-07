#pragma once

#include "gfx.h"

enum PixelFormat
{

	PIXELFORMAT_INDEXED8	= 	0,
	PIXELFORMAT_RGB16,
	PIXELFORMAT_RGB24,
	PIXELFORMAT_RGB32

};

//
//	A general framebuffer object. 
//
//	This contains a device independent framebuffer
//	and a set of function pointers to drawing routines.
//
//	The device dependent function pointers will point to 
// 	dummy functions and should be overridden by actual drivers,
//	such as the VESA driver.
//
//
struct FrameBuffer 
{
	// if subclassing, store pointer to class in here.
	void* privateData;	

	unsigned int width;
	unsigned int height;

	unsigned int rowStride;

	void*	pixelData;

	enum PixelFormat pixelFormat;

	bool (*activateFrameBufferDisplay)(struct FrameBuffer* fb);

	void (*clear)(struct FrameBuffer* fb, struct RGBColor* col);
	void (*swap)(struct FrameBuffer* fb);
};


struct FrameBuffer* gfx_fb_createFrameBuffer(unsigned int width, unsigned int height, unsigned int rowStride, enum PixelFormat pixelFormat);

