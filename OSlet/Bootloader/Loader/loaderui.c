
#include "loaderui.h"


bool loaderUI_init(struct FrameBuffer* fb)
{

	struct RGBColor col = {36, 84, 160};	

	fb->clear(fb, &col);
	
	fb->swap(fb);

}

