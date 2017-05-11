

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

void gfx_fb_blit_indexed8(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_blit_rgb16(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_blit_rgb24(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_blit_rgb32(struct FrameBuffer* fb, struct Image* im, int x, int y)
{

	switch (im->pixelFormat)
	{
		case PIXELFORMAT_RGB32:
		{

			unsigned int* dest = (unsigned int*)((uintptr_t)fb->pixelData + x*4 + y*fb->rowStride);

			unsigned int* src = (unsigned int*)im->data;

			for (int i = 0; i < im->height; i++)
			{
				lib_memcpy(dest, src, im->width*4);

				dest = (unsigned int*)((uintptr_t)dest + fb->rowStride);
				src  += im->width;
			}
			
			break;
		}

	}

};

void gfx_fb_alphaBlit_indexed8(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_alphaBlit_rgb16(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_alphaBlit_rgb24(struct FrameBuffer* fb, struct Image* im, int x, int y)
{
	// TODO
};

void gfx_fb_alphaBlit_rgb32(struct FrameBuffer* fb, struct Image* im, int x, int y)
{

	switch (im->pixelFormat)
	{
		case PIXELFORMAT_RGB32:
		{

			unsigned int* dest = (unsigned int*)((uintptr_t)fb->pixelData + x*4 + y*fb->rowStride);

			unsigned int* src = (unsigned int*)im->data;

			for (int i = 0; i < im->height; i++)
			{
				for (int j = 0; j < im->width; j++)
				{
	
					unsigned int destPixel = *(dest + j);
	
					unsigned int pixel = *(src + j);

					unsigned int alpha = ((pixel & 0xff000000) >> 24) + 1;
					unsigned int red   = (pixel & 0x00ff0000) >> 16;
					unsigned int green = (pixel & 0x0000ff00) >> 8;
					unsigned int blue  = (pixel & 0x000000ff);

					red *= alpha;
					green *= alpha;
					blue  *= alpha;

					unsigned int oneMinusAlpha = 0x100 - alpha;

					unsigned int destRed   = (destPixel & 0x00ff0000) >> 16;
					unsigned int destGreen = (destPixel & 0x0000ff00) >> 8;
					unsigned int destBlue  = (destPixel & 0x000000ff);
					
					destRed *= oneMinusAlpha;
					destGreen *= oneMinusAlpha;
					destBlue  *= oneMinusAlpha;

					red = (red + destRed) >> 8;
					green = (green + destGreen) >> 8;
					blue = (blue + destBlue) >> 8;

					unsigned int outPixel = (red << 16) | (green << 8) | (blue << 0); 	

					*(dest + j) = outPixel;

				}

				dest = (unsigned int*)((uintptr_t)dest + fb->rowStride);
				src  += im->width;
			}
			
			break;
		}

	}

};

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
			fb->blit  = gfx_fb_blit_indexed8;
			fb->alphaBlit  = gfx_fb_alphaBlit_indexed8;
			break;
		case PIXELFORMAT_RGB16:
			fb->clear = gfx_fb_clear_rgb16;
			fb->blit  = gfx_fb_blit_rgb16;
			fb->alphaBlit  = gfx_fb_alphaBlit_rgb16;
			break;
		case PIXELFORMAT_RGB24:
			fb->clear = gfx_fb_clear_rgb24;
			fb->blit  = gfx_fb_blit_rgb24;
			fb->alphaBlit  = gfx_fb_alphaBlit_rgb24;
			break;
		case PIXELFORMAT_RGB32:
			fb->clear = gfx_fb_clear_rgb32;
			fb->blit  = gfx_fb_blit_rgb32;
			fb->alphaBlit  = gfx_fb_alphaBlit_rgb32;
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

