
#include "hardware.h"

int gSomeGlobal = 320*200;
int gRandSeed = 0xdeadbeef;

typedef struct point_t
{
	float x;
	float y;
} point;


void mainExit();
void putPixel(int x, int y, char color);
int  randGen();

point midPoint(point* v1, point* v2);

// entry point of our flat binary. MUST be first function defined in our c-file
int start(void)
{
	// We have to set up segment descriptors
	// and set a stack, or else nothing will work.
	// THIS HAS TO BE THE FIRST THING WE DO.
	INIT_REGISTERS;


	point p1 = {0.0, 0.14};
	point p2 = {1.0, 0.14};
	point p3 = {0.5, 0.880};
	
	point v = {0.0,0.14};

	// clear the video mem, and test that our globals work.
	char *vidMem = (char*)VIDEOMEM;
	for (int i = 0; i < gSomeGlobal; i ++)
		vidMem[i] = 0x00;

	// Generate the Sierpinski triangle.
	for (int i = 0; i < 100000; i ++)
	{
		int d = randGen() % 3;

		if (d == 0)
		   v = midPoint(&p1, &v);
		else if (d == 1)
		   v = midPoint(&p2, &v);
		else if (d == 2)
		   v = midPoint(&p3, &v);
		
		putPixel((int)(v.x * 320.0), (int)((1.0 - v.y) * 200.0), 36);

	}

	mainExit();
}

void putPixel(int x, int y, char color)
{

	char *vidMem = (char*)VIDEOMEM;

	int vy = y * 320;

	vidMem[x + vy] = color;
}

int randGen()
{
	gRandSeed = gRandSeed * 1664525 + 1013904223;

	return gRandSeed;
}

point midPoint(point* v1, point* v2)
{
	point v;

	v.x = 0.5*(v1->x + v2->x);
	v.y = 0.5*(v1->y + v2->y);
	
	return v;
}

void mainExit()
{
myLoop:
	asm("hlt");
	goto myLoop;
}


