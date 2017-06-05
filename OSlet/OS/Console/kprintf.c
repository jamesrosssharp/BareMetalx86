
#include <stdarg.h>

#include "kprintf.h"
#include "console.h"

enum	FORMAT_CHARS 
{
	FORMAT_NONE,
	FORMAT_LONGLONG_HEX,
	FORMAT_HEX,
	FORMAT_DEC,
	FORMAT_FLOAT,
	FORMAT_STRING,
	FORMAT_CHAR,
	FORMAT_PERCENT_ESCAPED,
};


static	void 	parseFormatString(char **str, enum FORMAT_CHARS* format, bool* padWithZeros, int* preferredLength, int* decimalPlaces);

static void convertInt(int num, int base, bool padWithZeros, int preferredLength);
static void convertDouble(double real, bool padWidthZeros, int preferredLength, int decimalPlaces);

static bool isDigit(char c);


void 	kprintf(char* format, ...)
{

	va_list arg;
	va_start(arg, format);

	char* str = format;

	while (*str != 0)
	{

		if (*str == '%')
		{

			bool padWithZeros = false;
			int  preferredLength = 0;
			int  decimalPlaces = 0;
			enum FORMAT_CHARS format = FORMAT_NONE;	

			parseFormatString(&str, &format, &padWithZeros, &preferredLength, &decimalPlaces);

			switch(format)
			{
				case FORMAT_HEX:
				{	
					int hex = va_arg(arg, int);

					convertInt(hex, 16, padWithZeros, preferredLength);
	
					break;
				}
				case FORMAT_LONGLONG_HEX:
				{	
					unsigned long long int hex = va_arg(arg, unsigned long long int);

					convertInt(hex >> 16, 16, true, 8);
					convertInt(hex & 0xffffffff, 16, true, 8);	

					break;
				}
				case FORMAT_DEC:
				{
					int num = va_arg(arg, int);

					convertInt(num, 10, padWithZeros, preferredLength);

					break;
				}
				case FORMAT_FLOAT:
				{
					double real = va_arg(arg, double);

					convertDouble(real, padWithZeros, preferredLength, decimalPlaces);

					break;
				}
				case FORMAT_STRING:
				{
					char* string = va_arg(arg, char*);

					console_puts(string);

					break;
				}
				case FORMAT_CHAR:
				{
					int c = va_arg(arg, int);

					console_putch((char)c);

					break;
				}
				case FORMAT_PERCENT_ESCAPED:
				{
					console_putch('%');
					break;
				}
				default:
					break;
			}
		}
		else
		{
			console_putch(*str);
		}

		str++;

	}

	va_end(arg);

}

void 	parseFormatString(char **str, enum FORMAT_CHARS* format, bool* padWithZeros, int* preferredLength, int* decimalPlaces)
{

	enum State {
		START,
		GET_LONG,
		GET_DECIMAL_PLACES
	};

	enum State state = START;

	char* ptr = *str;
	
	ptr ++; // ignore % sign

	int pos = 0;
	*preferredLength = 0;
	*decimalPlaces = 0;

	*format = FORMAT_NONE;


	bool longlong = false;

	while (*ptr != 0)
	{
		if (isDigit(*ptr))
		{
			if (pos == 0 && *ptr == '0') 
				*padWithZeros = true;
			else {
			   if (state == START)
			   {
				*preferredLength *= 10;
			
				*preferredLength += (int)(*ptr - '0');
			   } 
			   else
			   {
				*decimalPlaces *= 10;
			
				*decimalPlaces += (int)(*ptr - '0');
			   }
			
			}
		}
		else if (*ptr == 'l')
		{
			if (state == GET_LONG)
			{
				longlong = true;
			}
			else
			{
				longlong = false;
			}

			state = GET_LONG;
		}	
		else if (*ptr == '.')
		{
			state = GET_DECIMAL_PLACES;
		}
		else if (*ptr == 'x')
		{
			if (longlong)
				*format = FORMAT_LONGLONG_HEX;
			else
				*format = FORMAT_HEX;
			break;
		}
		else if (*ptr == 'd')
		{
			*format = FORMAT_DEC;
			break;
		}
		else if (*ptr == 'c')
		{
			*format = FORMAT_CHAR;
			break;
		}
		else if (*ptr == 'f')
		{
			*format = FORMAT_FLOAT;
			break;
		}
		else if (*ptr == 's')
		{
			*format = FORMAT_STRING;
			break;
		} 
		else if (*ptr == '%')
		{
			*format = FORMAT_PERCENT_ESCAPED;
			break;
		}

		pos ++;
		ptr ++;

	}

	
	*str = ptr;
}

void convertInt(int num, int base, bool padWithZeros, int preferredLength)
{
	static char convertBuffer[] = "0123456789abcdef";
	
	if (base > 16) 
		return;

	if (base == 0)
		return;



	const int bufferSize = 32;

	char outputBuffer[bufferSize];

	for (int i = 0; i < bufferSize; i ++)
	{
		outputBuffer[i] = '0';
	}
	
	int idx = 0;


	unsigned int unsignedNum = (unsigned int)num;

	if (num < 0 && base == 10)
	{
		unsignedNum = (unsigned int)-num;
		console_putch('-');	
	} 

	while (unsignedNum != 0)
	{
		outputBuffer[idx] = convertBuffer[(unsignedNum % base)];
		unsignedNum /= base;
		
		idx ++;
	}
	
	// find maximal length of the string.

	int maxLen;

	for (maxLen = bufferSize - 1 ; maxLen > 0; maxLen --)
	{
		if (outputBuffer[maxLen] != '0')
			break;
	}

	// print number

	if (preferredLength > maxLen)
		maxLen = preferredLength - 1;

	for (int i = maxLen; i >= 0; i--)
	{
		console_putch(outputBuffer[i]);
	}

}

void convertDouble(double real, bool padWidthZeros, int preferredLength, int decimalPlaces)
{
}

bool isDigit(char c)
{

	return (c >= '0' && c <= '9');	
}

