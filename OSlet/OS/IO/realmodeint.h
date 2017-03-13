
#pragma once

#include "../kerndefs.h"

struct	__attribute__((__packed__)) RegisterDescription
{

	unsigned int EAX;
	unsigned int EBX;
	unsigned int ECX;
	unsigned int EDX;

	unsigned short DS;
	unsigned short ES;
	unsigned short FS;
	unsigned short GS;

	unsigned int ESI;
	unsigned int EDI;
	unsigned int EBP;

};

bool	io_realModeInt(unsigned char interrupt, struct RegisterDescription* inRegisters, struct RegisterDescription* outRegisters);

 
