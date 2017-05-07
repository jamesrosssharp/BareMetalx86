

#include "fb.h"
#include "../Mem/mem.h"
#include "../Console/console.h"

unsigned int gfx_fb_bytesPerPixel(enum PixelFormat pixelFormat)
{

	switch (pixelFormat)
	{
		case PIXELFORMAT_INDEXED8:
			return 1;
		case PIXELFORMAT_RGB16:
			return 2;
		case PIXELFORMAT_RGB24:
			return 3;
		case PIXELFORMAT_RGB32:
			return 4;
		default:
			return 0;
	}

	// shouldn't get here.
	return 0;

}

bool gfx_fb_activateFrameBufferDisplay(struct FrameBuffer* fb)
{
	// NOP
}

void gfx_fb_clear_indexed8(struct FrameBuffer* fb, struct RGBColor* col)
{
	
	// not implemented yet

}

void gfx_fb_clear_rgb16(struct FrameBuffer* fb, struct RGBColor* col)
{
	
	// not implemented yet

}

void gfx_fb_clear_rgb24(struct FrameBuffer* fb, struct RGBColor* col)
{
	
	// not implemented yet

}

void gfx_fb_clear_rgb32(struct FrameBuffer* fb, struct RGBColor* col)
{
	
	// not implemented yet

	unsigned int colBytes = gfx_prim_rgbColTo32(col);

	unsigned int* pixData = (unsigned int*)fb->pixelData;

	for (int j = 0; j < fb->height; j ++)
	{
		for (int i = 0; i < fb->width; i++)
				pixData[i] = colBytes;
	
		pixData = (unsigned int*)((uintptr_t)pixData + fb->rowStride);
	}

}

void gfx_fb_swap(struct FrameBuffer* fb)
{
	// NOP
}

struct FrameBuffer* gfx_fb_createFrameBuffer(unsigned int width, unsigned int height, unsigned int rowStride, enum PixelFormat pixelFormat)
{

	struct FrameBuffer* fb = NULL;

	unsigned int contigBytes = rowStride * height;

	if (contigBytes == 0)
		goto error;

	fb = (struct FrameBuffer*) kmalloc(sizeof(struct FrameBuffer), MEMORYTYPE_HIMEM);

	if (fb == NULL)
		goto error;

	kprintf("alloc'd framebuffer\n");

	fb->width = width;
	fb->height = height;
	fb->rowStride = rowStride;
	fb->pixelFormat = pixelFormat;

	// allocate actual frameBuffer

	fb->pixelData = kmalloc(contigBytes, MEMORYTYPE_HIMEM);	

	if (fb->pixelData == NULL)
		goto error;

	kprintf("alloc'd fbData\n");

	// fill in function pointers

	fb->activateFrameBufferDisplay = gfx_fb_activateFrameBufferDisplay;
	fb->swap		       = gfx_fb_swap;

	// pixel format dependant operations

	switch (fb->pixelFormat)
	{
		case PIXELFORMAT_INDEXED8:
			fb->clear = gfx_fb_clear_indexed8;
			break;
		case PIXELFORMAT_RGB16:
			fb->clear = gfx_fb_clear_rgb16;
			break;
		case PIXELFORMAT_RGB24:
			fb->clear = gfx_fb_clear_rgb24;
			break;
		case PIXELFORMAT_RGB32:
			fb->clear = gfx_fb_clear_rgb32;
			break;
		default:
			goto error;
	}

	return fb;

error:

	if (fb != NULL)
	{
		if (fb->pixelData != NULL)
			kfree(fb->pixelData);
	
		kfree(fb);
	}

	return NULL;

}

