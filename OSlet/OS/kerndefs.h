#pragma once

#ifndef NULL
#define NULL	0L
#endif

#define MIN(x,y)	( (x) <= (y) ? (x) : (y) )
#define MAX(x,y)	( (x) >= (y) ? (x) : (y) )

#define IS_POWER_OF_TWO(x) ((x & (x-1)) == 0)

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define ALIGNTO(x,y) (((x) + (y-1)) & ~(y-1))

#define KERNEL_LOAD_ADDRESS 	0x100000

#define KERNEL_PAGE_SIZE	4096

// If we are unit testing the code, need special defines

#ifdef UNIT_TEST

	// test on 64 bit platform

	#ifdef QUIET
		#define DEBUG(...) 	
	#else
		#define DEBUG	printf
	#endif

	#include <stdio.h>
	#include <stdint.h>

#else
	// normal build
	#define DEBUG(...) 	
	typedef unsigned int uintptr_t;

	typedef unsigned int size_t;

	typedef unsigned int off_t;

#endif	/* UNIT_TEST */


#include <stdbool.h>
