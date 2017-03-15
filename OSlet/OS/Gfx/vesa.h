
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

struct __attribute__ ((packed)) ModeInfo
{
      unsigned short ModeAttributes;      
      unsigned char  WinAAttributes;      
      unsigned char  WinBAttributes;     
      unsigned short WinGranularity;    
      unsigned short WinSize;          
      unsigned short WinASegment;     
      unsigned short WinBSegment;    
      unsigned long  WinFuncPtr;    
      unsigned short BytesPerScanLine;
      unsigned short XResolution;    
      unsigned short YResolution;   
      unsigned char  XCharSize;    
      unsigned char  YCharSize;      
      unsigned char  NumberOfPlanes;    
      unsigned char  BitsPerPixel;     
      unsigned char  NumberOfBanks;   
      unsigned char  MemoryModel;    
      unsigned char  BankSize;          
      unsigned char  NumberOfImagePages; 
      unsigned char  Reserved_page;     
      unsigned char  RedMaskSize;      
      unsigned char  RedMaskPos;      
      unsigned char  GreenMaskSize;  
      unsigned char  GreenMaskPos;   
      unsigned char  BlueMaskSize;   
      unsigned char  BlueMaskPos;      
      unsigned char  ReservedMaskSize; 
      unsigned char  ReservedMaskPos;   
      unsigned char  DirectColorModeInfo; 
      unsigned long  PhysBasePtr;        
      unsigned long  OffScreenMemOffset; 
      unsigned short OffScreenMemSize;  
      unsigned char  Reserved[206];    
};


bool	gfx_detectVESAModes();
