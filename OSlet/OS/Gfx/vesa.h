
#pragma once

#include "gfx.h"

struct __attribute__ ((packed)) VESAInfo	
{ 
      unsigned char  VESASignature[4];
      unsigned short VESAVersion;
      unsigned long  OEMStringPtr;
      unsigned char  Capabilities[4];
      unsigned long  VideoModePtr;
      unsigned short TotalMemory;
      unsigned short OemSoftwareRev;
      unsigned long  OemVendorNamePtr;
      unsigned long  OemProductNamePtr;
      unsigned long  OemProductRevPtr;
      unsigned char  Reserved[222];
      unsigned char  OemData[256];
};

bool	gfx_detectVESAModes();
