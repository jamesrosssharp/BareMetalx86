
#include "vesa.h"
#include "../Console/console.h"
#include "../IO/realmodeint.h"

#define MAX_VESA_MODES	256

struct 	VESAInfo gVESAInfo;


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
		(unsigned char*)(((gVESAInfo.OEMStringPtr & 0xffff0000) >> 12)
			 | (gVESAInfo.OEMStringPtr & 0x0000ffff) ));
	kprintf("Total memory: %d\n",gVESAInfo.TotalMemory);

	kprintf("Video mode ptr: %x\n", gVESAInfo.VideoModePtr);

	kprintf("Detected VESA modes.\n");

	return true;
}
