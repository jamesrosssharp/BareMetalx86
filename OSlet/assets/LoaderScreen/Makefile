

ASSET_FILE=../../../assets/BootSplash.svg

LAYERS= BG LoadingTextShadow LoadingText ProgressBarShadow	\
	ProgressBar ProgressBarFill LogoShadow Logo

all:	bootsplash.c

IMAGES=logo-cropped.png BG-1px.png progress-l.png progress-r.png progress-c.png	\
		progress-thumb-l.png progress-thumb-r.png progress-thumb-c.png	\
		loading-text-cropped.png

GEN_C_SOURCE=../../scripts/genimageblob.py   

.layers: 	$(ASSET_FILE) Makefile 
	for LAYER in $(LAYERS); \
		do inkscape -z -jC -i $$LAYER $(ASSET_FILE) -e $$LAYER.png;\
	done
	touch .layers

logo-flat.png:	.layers
	convert BG.png LogoShadow.png Logo.png -layers merge PNG32:logo-flat.png	

logo-cropped.png: logo-flat.png
	convert logo-flat.png -crop 470x260+720+280 -resize %50 \
		-alpha reset PNG32:logo-cropped.png 

BG-1px.png: .layers
	convert BG.png -crop 1x1+720+280 PNG32:BG-1px.png

progress-flat.png: .layers 
	convert BG.png ProgressBarShadow.png ProgressBar.png -layers merge PNG32:progress-flat.png

progress-l.png: progress-flat.png 
	convert progress-flat.png -crop 44x78+666+530 -resize 50% PNG32:progress-l.png

progress-c.png: progress-flat.png 
	convert progress-flat.png -crop 1x78+710+530 -resize 50% PNG32:progress-c.png

progress-r.png: progress-l.png 
	convert progress-l.png -flop PNG32:progress-r.png

progress-thumb-l.png: .layers 
	convert ProgressBarFill.png -crop 16x22+692+559 -resize 55% PNG32:progress-thumb-l.png

progress-thumb-c.png: .layers 
	convert ProgressBarFill.png -crop 1x22+708+559 -resize 55% PNG32:progress-thumb-c.png

progress-thumb-r.png: progress-thumb-l.png 
	convert progress-thumb-l.png -flop PNG32:progress-thumb-r.png

loading-text-flat.png: .layers 
	convert BG.png LoadingTextShadow.png LoadingText.png -layers merge PNG32:loading-text-flat.png 	

loading-text-cropped.png: loading-text-flat.png
	convert loading-text-flat.png -crop 188x102+870+706 -resize 50% PNG32:loading-text-cropped.png

bootsplash.c: $(IMAGES) $(GEN_C_SOURCE)
	$(GEN_C_SOURCE) $(IMAGES) -o bootsplash -b loaderBlob	
						# generate bootsplash.c and 
						# bootsplash.h

clean:
	rm *.png .layers *.c *.h
	
