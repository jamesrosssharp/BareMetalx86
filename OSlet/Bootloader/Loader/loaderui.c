
#include "loaderui.h"
#include "bootsplash.h"


void loaderUI_drawProgressThumb(struct FrameBuffer* fb, int progX, int progY, int width)
{
	struct Image progL = {img_progress_thumb_l_width,
			      img_progress_thumb_l_height,
			      (void*)&loaderBlob[img_progress_thumb_l_blobOffset],
			      PIXELFORMAT_RGB32};
	struct Image progR = {img_progress_thumb_r_width,
			      img_progress_thumb_r_height,
			      (void*)&loaderBlob[img_progress_thumb_r_blobOffset],
			      PIXELFORMAT_RGB32};
	struct Image progC = {img_progress_thumb_c_width,
			      img_progress_thumb_c_height,
			      (void*)&loaderBlob[img_progress_thumb_c_blobOffset],
			      PIXELFORMAT_RGB32};
	int x = progX;


	fb->alphaBlit(fb, &progL, x, progY);

	x += progL.width;

	for (int i = 0; i < width; i++)
	{
		// need alpha blend.
		fb->alphaBlit(fb, &progC, x, progY);
		x ++;
	}

	fb->alphaBlit(fb, &progR, x, progY);

}

bool loaderUI_init(struct FrameBuffer* fb)
{

	unsigned char red   = loaderBlob[img_bg_1px_blobOffset + 2];
	unsigned char green = loaderBlob[img_bg_1px_blobOffset + 1];
	unsigned char blue  = loaderBlob[img_bg_1px_blobOffset];

	struct RGBColor col = {red, green, blue};	

	fb->clear(fb, &col);

	struct Image logo = {img_logo_cropped_width, 
			   img_logo_cropped_height,
			   (void*)&loaderBlob[img_logo_cropped_blobOffset],
			   PIXELFORMAT_RGB32};
	
	int logoX = fb->width / 2 - logo.width / 2 - logo.width / 40;
	int logoY = fb->height / 2 - logo.height / 2 - 100;

	fb->blit(fb, &logo, logoX, logoY);

	struct Image progL = {img_progress_l_width,
			      img_progress_l_height,
			      (void*)&loaderBlob[img_progress_l_blobOffset],
			      PIXELFORMAT_RGB32};
	struct Image progR = {img_progress_r_width,
			      img_progress_r_height,
			      (void*)&loaderBlob[img_progress_r_blobOffset],
			      PIXELFORMAT_RGB32};
	struct Image progC = {img_progress_c_width,
			      img_progress_c_height,
			      (void*)&loaderBlob[img_progress_c_blobOffset],
			      PIXELFORMAT_RGB32};

	int progW = 320; 
	
	int progX = fb->width / 2 - progL.width - progW/2;
	int progY = fb->height / 2 - progL.height / 2;

	fb->blit(fb, &progL, progX, progY);
	
	fb->blit(fb, &progR, progX + progL.width + progW, progY);

	for (int i = 0; i < progW; i ++)
		fb->blit(fb, &progC, progX + progL.width + i, progY);

	struct Image text =  {img_loading_text_cropped_width,
			      img_loading_text_cropped_height,
			      (void*)&loaderBlob[img_loading_text_cropped_blobOffset],
			      PIXELFORMAT_RGB32};

	int textX = fb->width / 2 - text.width / 2;
	int textY = fb->height / 2 - text.height / 2 + 60;

	fb->blit(fb, &text, textX, textY);

	int i = 0;

	while (1)
	{
		fb->blit(fb, &progL, progX, progY);
	
		fb->blit(fb, &progR, progX + progL.width + progW, progY);

		for (int i = 0; i < progW; i ++)
			fb->blit(fb, &progC, progX + progL.width + i, progY);

		loaderUI_drawProgressThumb(fb, progX + 13, progY + 14, i++ % 321);
		fb->swap(fb);
	}

}

