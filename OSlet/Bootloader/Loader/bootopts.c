
#include "../../OS/Console/console.h"
#include "../../OS/Lib/klib.h"
#include "../../OS/Gfx/gfx.h"

#include "bootopts.h"


static void tokenise(char* text, char* textEnd, char** lineStart, char** lineEnd, char splitChar)
{

	char* start = text;
	char* end = text;

	while (*start == splitChar && start < textEnd)
		start++;

	end = start;

	while (*end != splitChar && end < textEnd)
		end++;

	*lineStart = start;
	*lineEnd = end;
}

static bool parseDisplay(char* displayStr, char* displayStrEnd, unsigned int* xres, unsigned int* yres, unsigned int* bpp)
{

	if (lib_strncmp(displayStr, "MAX", 3) == 0)
	{
		*xres = GFX_XRESOLUTION_MAX;
		*yres = GFX_YRESOLUTION_MAX;
		*bpp  = GFX_BPP_MAX;
	}
	else
	{
		char* xStart,* xEnd;
		char* yStart,* yEnd;
		char* bStart,* bEnd;

		tokenise(displayStr, displayStrEnd, &xStart, &xEnd, 'X');
		tokenise(xEnd, displayStrEnd, &yStart, &yEnd, 'X');
		tokenise(yEnd + 1, displayStrEnd, &bStart, &bEnd, '\0');

		unsigned int xLen = xEnd - xStart;
		unsigned int yLen = yEnd - yStart;
		unsigned int bLen = bEnd - bStart;

		if (xLen == 0 || xLen > 4)
			return false;
		if (yLen == 0 || yLen > 4)
			return false;
		if (bLen == 0 || bLen > 2)
			return false;
	
		char xBuf[5];
		char yBuf[5];
		char bBuf[3];

		lib_memcpy(xBuf, xStart, xLen);
		lib_memcpy(yBuf, yStart, yLen);
		lib_memcpy(bBuf, bStart, bLen);

		xBuf[xLen] = '\0';
		yBuf[yLen] = '\0';
		bBuf[bLen] = '\0';

		*xres = lib_atoi(xBuf);
		*yres = lib_atoi(yBuf);
		*bpp  = lib_atoi(bBuf);

	}

	return true;
}

bool parseBootOpts(struct BootOpts* bootOpts, char* text, char* textEnd)
{

	char* lineStart,* lineEnd;	

	unsigned int lineNum = 1;

	enum State 
	{
		IDLE,
		READ_BOOTLOADER_OPTS,
		READ_KERNEL_OPTS
	};


	// set default values

	bootOpts->bootXRes = GFX_XRESOLUTION_MAX;
	bootOpts->bootYRes = GFX_YRESOLUTION_MAX;
	bootOpts->bootBpp  = GFX_BPP_MAX;

	bootOpts->kernelXRes = GFX_XRESOLUTION_MAX;
	bootOpts->kernelYRes = GFX_YRESOLUTION_MAX;
	bootOpts->kernelBpp  = GFX_BPP_MAX;

	// parse text

	enum State state = IDLE;
	
	while (1)
	{

		if (text == textEnd)
			break;

		tokenise(text, textEnd, &lineStart, &lineEnd, '\n');

		unsigned int lineLength = lineEnd - lineStart;

		const int lineBufLen = 80;

		char line[lineBufLen];

		lib_bzero(line, lineBufLen);

		lib_memcpy(line, lineStart, lineLength < lineBufLen - 1 ? lineLength : lineBufLen - 1);		

		lib_strtoupper(line);

		char* lineptr = line;

		lib_trim(&lineptr);	

		//kprintf("Line: !%s!\n", lineptr);

		if (lib_strncmp(lineptr, "[BOOTLOADER]", lineBufLen) == 0)
		{	
			state = READ_BOOTLOADER_OPTS;
		}
		else if (lib_strncmp(lineptr, "[KERNEL]", lineBufLen) == 0)
 		{
			state = READ_KERNEL_OPTS;
		}
		else if (lib_strncmp(lineptr, "", lineBufLen) == 0)
		{
			// Do nothing
		}
		else
		{

			char* keyStart,* keyEnd;

			char* valStart,* valEnd;

			char key[lineBufLen];
			char val[lineBufLen];

			lib_bzero(key, lineBufLen);
			lib_bzero(val, lineBufLen);
			
			tokenise(lineStart, lineEnd, &keyStart, &keyEnd, '=');

			tokenise(keyEnd, lineEnd, &valStart, &valEnd, '=');

			unsigned int keyLen = keyEnd - keyStart;
			unsigned int valLen = valEnd - valStart;

			if (keyLen == 0 || valLen == 0)
			{
				kprintf("Malformed line %d\n", lineNum);
				return false;
			}
			else
			{
				lib_memcpy(key, keyStart, (keyLen < lineBufLen - 1) ? keyLen : lineBufLen - 1);
				lib_memcpy(val, valStart, (valLen < lineBufLen - 1) ? valLen : lineBufLen - 1);

				char* keyPtr = key;
				char* valPtr = val;

				lib_trim(&keyPtr);	
				lib_trim(&valPtr);

				lib_strtoupper(keyPtr);
				lib_strtoupper(valPtr);

				//kprintf("Key: %s Val: %s\n", keyPtr, valPtr);

				if (lib_strncmp(keyPtr, "DISPLAY", lineBufLen) == 0)
				{

					switch(state)
					{
						case READ_BOOTLOADER_OPTS:
							if (! parseDisplay(valPtr, valPtr + lineBufLen, 
								&bootOpts->bootXRes, &bootOpts->bootYRes, &bootOpts->bootBpp))
							{
								kprintf("Error parsing DISPLAY, line %d\n", lineNum);
								return false;
							}
						break;
						case READ_KERNEL_OPTS:
							if (! parseDisplay(valPtr, valPtr + lineBufLen,
									&bootOpts->kernelXRes, 
									&bootOpts->kernelYRes, &bootOpts->kernelBpp))
							{
								kprintf("Error parsing DISPLAY, line %d\n", lineNum);
								return false;
							}
						break;
						default:
							kprintf("Error: line %d: DISPLAY not in kernel or bootloader section.\n", lineNum);
							return false;
					}

				}
				else if (lib_strncmp(keyPtr, "FINDROOTBY", lineBufLen) == 0)
				{

					// TODO

				}
				else if (lib_strncmp(keyPtr, "ROOT", lineBufLen) == 0)
				{

					// TODO

				}

				
			}

		}

		lineNum++;
		text = lineEnd;

	}

	return true;

}
