
#pragma once

struct RGBColor
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};



unsigned int gfx_prim_rgbColTo32(struct RGBColor* col);

