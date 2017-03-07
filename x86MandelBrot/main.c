
#include "hardware.h"

int gSomeGlobal = 320*200;

#define MAX_ITER 254

typedef struct complex_t
{
	float real;
	float imag;
} complex_number;

int gRandSeed = 0xdeadbeef;

void mainExit();
void putPixel(int x, int y, char color);

complex_number complex_lerp(complex_number c1, complex_number c2, float x, float y);
float complex_norm_squared(complex_number c1);
complex_number complex_sum(complex_number c1, complex_number c2);
complex_number complex_mul(complex_number c1, complex_number c2);

void sleep();

int	mandelbrot_iter(complex_number c, complex_number z);


void renderMandelBrot();
void renderQuadraticJulia();

int  randGen();

void setPalette();

void outp(unsigned short port, unsigned char byte);

// entry point of our flat binary. MUST be first function defined in our c-file
int start(void)
{
	// We have to set up segment descriptors
	// and set a stack, or else nothing will work.
	// THIS HAS TO BE THE FIRST THING WE DO.
	INIT_REGISTERS;

	setPalette();

	// clear the video mem, and test that our globals work.
	char *vidMem = (char*)VIDEOMEM;
	for (int i = 0; i < gSomeGlobal; i ++)
		vidMem[i] = 0x00;


	// generate a nice mandelbrot set

	while (1)
	{

		int d = randGen() % 2;

		if (d == 1)
			renderMandelBrot();
		else
			renderQuadraticJulia();


		sleep();

	}

	mainExit();
}

void renderQuadraticJulia()
{

	complex_number p1 = {-2.0, -2.0};
	complex_number p2 = {2.0, 2.0};

	complex_number c;

	c.real = (float)(randGen() % 10000) / 10000.0 - 0.5;
	c.imag = (float)(randGen() % 10000) / 10000.0 - 0.5;



	for (int j = 0; j < 200; j ++)
	{
		for (int i = 0; i < 320; i ++)
		{

			complex_number z = complex_lerp(p1, p2, i / 320.0, j / 200.0);
	


			int iter = mandelbrot_iter(c, z);	

			if (iter < MAX_ITER)
				putPixel(i, j, (char)iter + 1);
			else
				putPixel(i, j, 0);
		}
	}



}

void renderMandelBrot()
{
	
	while (1) 
	{

		// choose two points at random

		complex_number p1, p2, p3;

		p1.real = (float)(randGen() % 40000) / 10000.0 - 2.0;
		p1.imag = (float)(randGen() % 40000) / 10000.0 - 2.0;

		p3.real = (float)(randGen() % 4000000) / 4000000.0;
		p3.imag = p3.real;

		p2 = complex_sum(p1, p3);


		// compute mandelbrot iterations for each corner

		int m1, m2, m3, m4;

		complex_number p4, p5;

		p4.real = p1.real;
		p4.imag = p2.imag;

		p5.real = p2.real;
		p5.imag = p1.imag;

		m1 = mandelbrot_iter(p1, p1);
		m2 = mandelbrot_iter(p2, p2);
		m3 = mandelbrot_iter(p4, p4);
		m4 = mandelbrot_iter(p5, p5);
	

		// if they are all different, render the fractal

		if (m1 == m2 == m3 == m4)
			continue;

		if (m1 > MAX_ITER || m2 > MAX_ITER || m3 > MAX_ITER || m4 > MAX_ITER)
			continue;

		if (m1 == m2 || m1 == m3 || m1 == m4 || m2 == m3 || m2 == m4 ||
			m3 == 4)
			continue;

		if ((m1 - m2) * (m1 - m2) < 16)
			continue;

		if ((m1 - m3) * (m1 - m3) < 16)
			continue;

		if ((m1 - m4) * (m1 - m4) < 16)
			continue;

		if ((m2 - m3) * (m2 - m3) < 16)
			continue;

		if ((m2 - m4) * (m2 - m4) < 16)
			continue;

		if ((m3 - m4) * (m3 - m4) < 16)
			continue;


		for (int j = 0; j < 200; j ++)
		{
			for (int i = 0; i < 320; i ++)
			{
	
				complex_number c = complex_lerp(p1, p2, i / 320.0, j / 200.0);
		
				int iter = mandelbrot_iter(c, c);	

				if (iter < MAX_ITER)
					putPixel(i, j, (char)iter + 1);
				else
					putPixel(i, j, 0);
			}
		}

		break;

	}
}


void putPixel(int x, int y, char color)
{

	char *vidMem = (char*)VIDEOMEM;

	int vy = y * 320;

	vidMem[x + vy] = color;
}

complex_number complex_lerp(complex_number c1, complex_number c2, float x, float y)
{
	complex_number c;

	c.real = c1.real + (c2.real - c1.real) * x;
	c.imag = c1.imag + (c2.imag - c1.imag) * y;

	return c;

}


int	mandelbrot_iter(complex_number c, complex_number z)
{

	int iter = 0;

	while (complex_norm_squared(z) < 4 && iter < MAX_ITER)
	{
		z = complex_sum(complex_mul(z, z), c); 

		iter ++;
	}
	
	return iter;
}

float complex_norm_squared(complex_number c1)
{

	float norm = c1.real * c1.real + c1.imag * c1.imag;

	return norm;

}

complex_number complex_sum(complex_number c1, complex_number c2)
{

	complex_number c;

	c.real = c1.real + c2.real;
	c.imag = c1.imag + c2.imag;

	return c;
}

complex_number complex_mul(complex_number c1, complex_number c2)
{

	complex_number c;

	c.real = c1.real*c2.real - c1.imag*c2.imag;
	c.imag = c1.real*c2.imag + c1.imag*c2.real;

	return c;

}

void sleep()
{

	for (int i = 0; i < 0x7ffffff; i++)
		for (int j = 0; j < 0x1; j++)
				{
					asm("nop");				
				}
	

}

int randGen()
{
	gRandSeed = gRandSeed * 1664525 + 1013904223;

	return gRandSeed;
}

void setPalette()
{

	// set palette index
	outp(0x03c8, 0);

	
	outp(0x03c9, 255);
	outp(0x03c9, 255);
	outp(0x03c9, 255);	
}

void outp(unsigned short port, unsigned char byte)
{
	__asm__ volatile (
	    " mov %%eax, %0 \n" // these instructions are quite irrelevant 
	    " mov %%edx, %1 \n" // if you look at the disassembly.
	    " out %%al, %%dx \n"
		:
		: "r" ((unsigned int)byte),
		  "r" ((unsigned int)port)
	    );
}

void mainExit()
{
myLoop:
	asm("hlt");
	goto myLoop;
}


