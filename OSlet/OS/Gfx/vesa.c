
#include "vesa.h"
#include "../Console/console.h"
#include "../IO/realmodeint.h"

#define MAX_VESA_MODES	256

#define CONVERT_REAL_MODE_ADDRESS(x)	((x & 0xffff0000) >> 12) | (x & 0x0000ffff)

struct 	VESAInfo gVESAInfo;

struct 	ModeInfo gVESAModeInfo[MAX_VESA_MODES];

int	gTotalSupportedVESAModes;

bool	gfx_detectVESAModes()
{

	kprintf("Detecting VESA modes...\n");

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

	kprintf("VESA Signature: %c%c%c%c (%x)\n", gVESAInfo.VESASignature[0],
		gVESAInfo.VESASignature[1],
		gVESAInfo.VESASignature[2],
		gVESAInfo.VESASignature[3],
		&gVESAInfo
	);

	kprintf("OEM String: %s\n",
		(unsigned char*)( CONVERT_REAL_MODE_ADDRESS(gVESAInfo.OEMStringPtr)));
	kprintf("Total memory: %d kB\n",gVESAInfo.TotalMemory * 64);

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

		kprintf("Detected mode: 0x%x %dx%d, %dbpp\n", *modes, 
				modeInfo->XResolution, 
				modeInfo->YResolution,
				modeInfo->BitsPerPixel);
		modes ++;
		gTotalSupportedVESAModes ++;
		modeInfo ++;
	}


	kprintf("Detected %d VESA modes.\n", gTotalSupportedVESAModes);

	return true;
}
