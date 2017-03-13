
#include "cpu.h"

char gCPUString[13] = {'N','o','t',' ','D','e','t','e','c','t','e','d',0};

unsigned int gCPUFeatECX = 0;
unsigned int gCPUFeatEDX = 0;

bool gCPUDetected = false;

bool	io_CPUSupportsCPUID();
void	io_queryCPUIdString(char* string);
void	io_queryCPUFeatures(unsigned int* edx, unsigned int* ecx);			

bool io_detectCPU()
{

	kprintf("Detecting CPU...\n");

	bool	supportsCPUID = io_CPUSupportsCPUID();

	if (supportsCPUID)
	{
	
		kprintf("About to query CPU id\n");
	
		// Query CPU id string

		io_queryCPUIdString(gCPUString);
		kprintf("CPU String: %s\n", gCPUString);

		// Query CPU features

		io_queryCPUFeatures(&gCPUFeatEDX, &gCPUFeatECX);			
		if (gCPUFeatEDX & CPUID_FEAT_EDX_APIC)
			kprintf("CPU is APIC enabled\n");
		else
			kprintf("CPU is not APIC enabled\n");  


		if (gCPUFeatEDX & CPUID_FEAT_EDX_ACPI)
			kprintf("CPU has ACPI\n");
		else
			kprintf("CPU does not have ACPI\n");

		gCPUDetected = true;

	}
	else
	{
		kprintf("CPU does not support CPUID.");	
	}

	kprintf("Detected CPU.\n");

	return true;

}

bool	io_CPUSupportsCPUID()
{

	int flags = 0;

	__asm__ volatile 
	(	
		"pushf\n"                       
    		"pushf\n"                      
    		"xorl $0x00200000,(%%esp)\n"           
    		"popf\n"
                "pushf\n"
                "pop %%eax\n"
                "xor (%%esp),%%eax\n"                        
   		"popf\n"                             
    		"andl $0x00200000,%%eax\n" 
		"mov %0, %%eax\n" 
		: "=r" (flags)
		:  	
	);

	return (flags != 0);
	
}


void	io_queryCPUIdString(char* string)
{


	int ebx = 0;
	int ecx = 0;
	int edx = 0;

	__asm__ volatile (
		"xor %%eax, %%eax\n"
		"cpuid\n"
		: "=b" (ebx), "=c" (ecx), "=d" (edx)
		:
		: "eax"
	);


	int idx = 0;

	unsigned int regs[] = {ebx, edx, ecx};

	for (int i = 0; i < 3; i ++)
	{

		unsigned int reg = regs[i];

		string[idx++] = reg & 0xff;
		reg >>= 8;
		string[idx++] = reg & 0xff;
		reg >>= 8;
		string[idx++] = reg & 0xff;
		reg >>= 8;
		string[idx++] = reg & 0xff;

	}
	
}

void	io_queryCPUFeatures(unsigned int* outEdx, unsigned int* outEcx)
{
	
	int ecx = 0;
	int edx = 0;

	__asm__ volatile (
		"xor %%eax, %%eax\n"
		"inc %%eax\n"
		"cpuid\n"
		: "=c" (ecx), "=d" (edx)
		:
		: "eax"
	);

	*outEdx = edx;
	*outEcx = ecx;

}			
