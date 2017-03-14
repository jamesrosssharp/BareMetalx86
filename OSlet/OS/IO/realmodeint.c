

#include "realmodeint.h"


// Function prototype for assembly function
 int boot_realModeIntWrapper(unsigned char interrupt, struct RegisterDescription *inRegisters, struct RegisterDescription *outRegisters);	

bool	io_realModeInt(unsigned char interrupt, struct RegisterDescription* inRegisters, struct RegisterDescription* outRegisters)
{

	int ret;

	//kprintf("calling real mode wrapper EAX=%08x EBX=%08x ECX=%08x EDX=%08x \n", 
	//		inRegisters->EAX,  inRegisters->EBX,  inRegisters->ECX,  inRegisters->EDX );

	ret = boot_realModeIntWrapper(interrupt, inRegisters, outRegisters);	

	//kprintf("back in protected mode EAX=%08x EBX=%08x ECX=%08x EDX=%08x \n", 
	//	outRegisters->EAX,  outRegisters->EBX,  outRegisters->ECX,  outRegisters->EDX);

	return (ret != 0);

}

/*
int boot_realModeIntWrapper(unsigned char interrupt, struct RegisterDescription *inRegisters, struct RegisterDescription *outRegisters)
{

	__asm__ volatile (
		"nop\n"
		: 
		: "a" (interrupt), "b" (inRegisters), "c" (outRegisters)
	);

	return false;
}
*/


