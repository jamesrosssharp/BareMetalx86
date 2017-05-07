
#include "vesa.h"
#include "../Console/console.h"
#include "../IO/realmodeint.h"
#include "../Mem/mem.h"

#define MAX_VESA_MODES	256

#define CONVERT_REAL_MODE_ADDRESS(x)	((x & 0xffff0000) >> 12) | (x & 0x0000ffff)

struct 	VESAInfo gVESAInfo;

struct 	ModeInfo gVESAModeInfo[MAX_VESA_MODES];
int	gVESAModeIndex[MAX_VESA_MODES];

int	gTotalSupportedVESAModes = 0;

bool	gfx_detectVESAModes()
{

	DEBUG("Detecting VESA modes...\n");

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	in.ES = (((int)&gVESAInfo) & 0xffff0000) >> 4;
	in.EDI = (((int)&gVESAInfo) & 0x0000ffff); 
	in.EAX = 0x4f00;	

	io_realModeInt(0x10, &in, &out);	

	if (out.EAX & 0x0000ff00)
	{
		kprintf("Could not get VESA info block");
		return false;
	}

	DEBUG("VESA Signature: %c%c%c%c (%x)\n", gVESAInfo.VESASignature[0],
		gVESAInfo.VESASignature[1],
		gVESAInfo.VESASignature[2],
		gVESAInfo.VESASignature[3],
		&gVESAInfo
	);

	DEBUG("OEM String: %s\n",
		(unsigned char*)( CONVERT_REAL_MODE_ADDRESS(gVESAInfo.OEMStringPtr)));
	DEBUG("Total memory: %d kB\n",gVESAInfo.TotalMemory * 64);

	unsigned short* modes = (unsigned short*)(CONVERT_REAL_MODE_ADDRESS(gVESAInfo.VideoModePtr));

	gTotalSupportedVESAModes = 0;

	struct ModeInfo* modeInfo = gVESAModeInfo;

	while(*modes != 0xffff && gTotalSupportedVESAModes < MAX_VESA_MODES)
	{

		// Get mode info

		in.ES = (((int)modeInfo) & 0xffff0000) >> 4;
		in.EDI = (((int)modeInfo) & 0x0000ffff); 
		in.EAX = 0x4f01;	
		in.ECX = (unsigned int)*modes;

		io_realModeInt(0x10, &in, &out);	

		if (out.EAX & 0x0000ff00)
		{
			kprintf("Could not get mode info block");
			return false;
		}

		DEBUG("Detected mode: 0x%x %dx%d, %dbpp\n", *modes, 
				modeInfo->XResolution, 
				modeInfo->YResolution,
				modeInfo->BitsPerPixel);

		gVESAModeIndex[gTotalSupportedVESAModes] = *modes;

		modes ++;
		gTotalSupportedVESAModes ++;
		modeInfo ++;
	}


	DEBUG("Detected %d VESA modes.\n", gTotalSupportedVESAModes);

	return true;
}


bool	gfx_vesa_findCompatibleMode(int* mode, int* xres, int* yres, int* bpp)
{

	int foundMode = -1;
	int foundXRes = 0;
	int foundYRes = 0;
	int foundBPP  = 0;
	
	for (int i = 0; i < gTotalSupportedVESAModes; i++)
	{

		if ( (gVESAModeInfo[i].XResolution == *xres) &&
		     (gVESAModeInfo[i].YResolution == *yres) &&
		     (gVESAModeInfo[i].BitsPerPixel == *bpp) )
		{
			foundMode = i;
			foundXRes = gVESAModeInfo[i].XResolution;
			foundYRes = gVESAModeInfo[i].YResolution;
			foundBPP  = gVESAModeInfo[i].BitsPerPixel;
 			break;	
		}
		else if (((*xres == GFX_XRESOLUTION_MAX) && (gVESAModeInfo[i].XResolution >= foundXRes)) && 
			 ((*yres == GFX_YRESOLUTION_MAX) && (gVESAModeInfo[i].YResolution >= foundYRes)) && 
			 ((*bpp == GFX_BPP_MAX) && (gVESAModeInfo[i].XResolution >= foundBPP))) 
		{
	
			DEBUG("Mode: %d %d %d %d\n", i, gVESAModeInfo[i].XResolution, gVESAModeInfo[i].YResolution, gVESAModeInfo[i].BitsPerPixel);

			foundMode = i;
			foundXRes = gVESAModeInfo[i].XResolution;
			foundYRes = gVESAModeInfo[i].YResolution;
			foundBPP  = gVESAModeInfo[i].BitsPerPixel;
		}

	}

	if (foundMode < 0)
		return false;

	*xres = foundXRes;
	*yres = foundYRes;
	*bpp  = foundBPP;
	*mode = foundMode;

	return true;
}

bool	gfx_vesa_setVideoMode(int mode)
{

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	in.EAX = 0x4f02;
	in.EBX = gVESAModeIndex[mode];

	kprintf("Setting mode: %d\n", mode);
	
	io_realModeInt(0x10, &in, &out);	

	if (out.EAX & 0xff00)
		return false;

	return true;

}

void 	gfx_vesa_setBank(unsigned int bank)
{

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	in.EAX = 0x4f05;
	in.EBX = 0;
	in.EDX = bank;

	io_realModeInt(0x10, &in, &out);	

}

void 	gfx_vesa_swapFrameBuffer(struct FrameBuffer* fb)
{

	// swap display from linear device independent frame buffer
	// to banked VESA gfx mem

	struct VesaFrameBuffer* vfb = fb->privateData;

	unsigned int bytesToCopy = fb->rowStride * fb->height;
	unsigned int bank = 0;

	unsigned char* pixelData = (unsigned char*)fb->pixelData;

	unsigned char* windowPtr = (unsigned char*)0xa0000;

	unsigned int bankSize = vfb->modeInfo->WinSize * 1024;
	unsigned int bankGranularity = vfb->modeInfo->WinGranularity * 1024;

	while (bytesToCopy)
	{

		gfx_vesa_setBank(bank);

		int bytesThisBank = bytesToCopy > bankSize ? bankSize : bytesToCopy;
	
		lib_memcpy(windowPtr, pixelData, bytesThisBank);

		bytesToCopy -= bytesThisBank;

		bank += bankSize / bankGranularity;

		pixelData += bytesThisBank;

	}
		
}

bool	gfx_vesa_activateFrameBufferDisplay(struct FrameBuffer* fb)
{

	struct VesaFrameBuffer* vfb = fb->privateData;

	return gfx_vesa_setVideoMode(vfb->mode);


}

struct FrameBuffer* gfx_vesa_createFrameBuffer(int mode)
{

	struct FrameBuffer* fb = NULL;

	int width;
	int height;

	enum PixelFormat pixelFormat;

	struct VesaFrameBuffer* vfb = (struct VesaFrameBuffer*)kmalloc(sizeof(struct VesaFrameBuffer), MEMORYTYPE_HIMEM); 

	if (vfb == NULL)
		goto error;

	kprintf("alloc'd VesaFrameBuffer\n");

	struct ModeInfo* modeInfo = &gVESAModeInfo[mode];	

	width = modeInfo->XResolution;
	height = modeInfo->YResolution;
	
	switch	(modeInfo->BitsPerPixel)
	{
		case 8:
			pixelFormat = PIXELFORMAT_INDEXED8;
			break;
		case 16:
			pixelFormat = PIXELFORMAT_RGB16;
			break;
		case 24:
			pixelFormat = PIXELFORMAT_RGB24;
			break;
		case 32:
			pixelFormat = PIXELFORMAT_RGB32;
			break;
		default:
			goto error;
	}	


	vfb->mode = mode;
	vfb->modeInfo = modeInfo;

	fb = gfx_fb_createFrameBuffer(width, height, modeInfo->BytesPerScanLine, pixelFormat);

	if (fb == NULL)
		goto error;

	kprintf("Alloc'd framebuffer\n");

	fb->privateData = vfb;

	// set function pointers

	fb->activateFrameBufferDisplay = gfx_vesa_activateFrameBufferDisplay;
	fb->swap  = gfx_vesa_swapFrameBuffer;

	//kprintf("window size: %08x window granularity: %08x\n", vfb->modeInfo->WinSize * 1024, vfb->modeInfo->WinGranularity * 1024);

	// success
	return fb;

error:

	if (vfb != NULL)
		kfree(vfb);

	if (fb != NULL)
		kfree(fb);

	return NULL;
}	
