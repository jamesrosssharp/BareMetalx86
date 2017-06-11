#pragma once

#include "../../OS/kerndefs.h"

struct BootOpts
{

	// boot loader options

	int bootXRes;
	int bootYRes;
	int bootBpp;


	// kernel options

	int kernelXRes;
	int kernelYRes;
	int kernelBpp;


};

bool parseBootOpts(struct BootOpts* bootOps, char* text, char* textEnd);
