
#include "primitives.h"


unsigned int gfx_prim_rgbColTo32(struct RGBColor* col)
{

	unsigned int bytes = 0;
	
	bytes |= col->red << 16;
	bytes |= col->green << 8;
	bytes |= col->blue  << 0;

	return bytes;
}
