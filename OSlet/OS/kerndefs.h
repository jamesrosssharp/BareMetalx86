#pragma once

#ifndef NULL
#define NULL	0L
#endif

#define MIN(x,y)	( (x) <= (y) ? (x) : (y) )
#define MAX(x,y)	( (x) >= (y) ? (x) : (y) )

#define IS_POWER_OF_TWO(x) ((x & (x-1)) == 0)

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define ALIGNTO(x,y) ((x + (y-1)) & ~(y-1))

// If we are unit testing the code, need special defines

#ifdef UNIT_TEST

	// test on 64 bit platform

	#define DEBUG	printf

	#include <stdio.h>
	#include <stdint.h>

#else
	// normal build
	#define DEBUG(...) 	
	typedef unsigned int uintptr_t;

#endif	/* UNIT_TEST */


#include <stdbool.h>
