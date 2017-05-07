

ASSET_FILE=../../../assets/BootSplash.svg

LAYERS= BG LoadingTextShadow LoadingText ProgressBarShadow	\
	ProgressBar ProgressBarFill LogoShadow Logo

all: logo-cropped.png BG-1px.png progress-l.png progress-r.png progress-c.png	\
		progress-thumb-l.png progress-thumb-r.png progress-thumb-c.png	\
		loading-text-cropped.png

.layers: 	$(ASSET_FILE) Makefile 
	for LAYER in $(LAYERS); \
		do inkscape -z -jC -i $$LAYER $(ASSET_FILE) -e $$LAYER.png;\
	done
	touch .layers

logo-flat.png:	.layers
	convert BG.png LogoShadow.png Logo.png -layers merge logo-flat.png	

logo-cropped.png: logo-flat.png
	convert logo-flat.png -crop 470x260+720+280 logo-cropped.png 

BG-1px.png: .layers
	convert BG.png -crop 1x1+720+280 BG-1px.png

progress-flat.png: .layers 
	convert BG.png ProgressBarShadow.png ProgressBar.png -layers merge progress-flat.png

progress-l.png: progress-flat.png 
	convert progress-flat.png -crop 44x78+666+530  progress-l.png

progress-c.png: progress-flat.png 
	convert progress-flat.png -crop 1x78+710+530 progress-c.png

progress-r.png: progress-l.png 
	convert progress-l.png -flop progress-r.png

progress-thumb-l.png: .layers 
	convert ProgressBarFill.png -crop 16x22+692+559 progress-thumb-l.png

progress-thumb-c.png: .layers 
	convert ProgressBarFill.png -crop 1x22+708+559 progress-thumb-c.png

progress-thumb-r.png: progress-thumb-l.png 
	convert progress-thumb-l.png -flop progress-thumb-r.png

loading-text-flat.png: .layers 
	convert BG.png LoadingTextShadow.png LoadingText.png -layers merge loading-text-flat.png 	

loading-text-cropped.png: loading-text-flat.png
	convert loading-text-flat.png -crop 188x102+870+706 loading-text-cropped.png

clean:
	rm *.png .layers
	
