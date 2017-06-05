
#include "unicodestring.h"
#include "../Mem/mem.h"
#include "../Console/console.h"

static bool growString(struct UnicodeString* thisString, unsigned int charsToAdd)
{

	if (charsToAdd + thisString->stringLength > thisString->wcharBufferLength)
	{
		// allocate more memory for the string
		
		
		unsigned int newLen = thisString->wcharBufferLength;

		while (newLen < thisString->stringLength + charsToAdd)
			newLen *= 2;

		unsigned short* theNewBuffer = kmalloc(newLen * sizeof(unsigned short), MEMORYTYPE_HIMEM);
	
		if (theNewBuffer == NULL)		
			return false;

		lib_memcpy(theNewBuffer, thisString->wcharBuffer, thisString->wcharBufferLength * sizeof(unsigned short));	

		kfree(thisString->wcharBuffer);
	
		thisString->wcharBuffer = theNewBuffer;
		thisString->wcharBufferLength = newLen;
	}

	return true;
}


static bool appendASCIICString(struct UnicodeString* thisString, const char* asciiString)
{
	
	int chars = lib_strlen(asciiString);

	if (! growString(thisString, chars))
		return false;

	unsigned short* buf = thisString->wcharBuffer + thisString->stringLength;

	for (int i = 0; i < chars; i++)
		*buf++ = (unsigned short)(asciiString[i] & 0x7f); 	

	thisString->stringLength += chars;

	return true;

}

static bool appendUTF16String(struct UnicodeString* thisString, const unsigned short* utf16string, unsigned int length)
{

	if (! growString(thisString, length))
		return false;

	unsigned short* buf = thisString->wcharBuffer + thisString->stringLength;

	for (int i = 0; i < length; i++)
		*buf++ = (unsigned short)(utf16string[i]); 	

	thisString->stringLength += length;

	return true;

}

static int compare(const struct UnicodeString* thisString, const struct UnicodeString* thatString)
{

	unsigned short* a;
	unsigned short* b;

	unsigned int lengtha, lengthb;

	if (! thisString->substringActive)
	{
		a = thisString->wcharBuffer;
		lengtha = thisString->stringLength;
	}
	else
	{
		a = &thisString->wcharBuffer[thisString->lowerLimit];
		lengtha = thisString->upperLimit - thisString->lowerLimit;	
	}

	if (! thatString->substringActive)
	{
		b = thatString->wcharBuffer;
		lengthb = thatString->stringLength;
	}
	else
	{
		b = &thatString->wcharBuffer[thatString->lowerLimit];
		lengthb = thatString->upperLimit - thatString->lowerLimit;	
	}


	// strings can't compare equal if they're of different length
	if (lengtha > lengthb)
		return 1;

	if (lengtha < lengthb)
		return -1;


	while (lengtha --) 
	{

		if (*a != *b) 
			return *a - *b; 

		++a; ++b;
	}
  
	return 0;	
}

static bool toASCIICString(struct UnicodeString* thisString, char* dest, unsigned int destSize)
{

	int i;

	bool fullyConverted = true;

	unsigned int start, end;

	if (thisString->substringActive)
	{
		start = thisString->lowerLimit;
		end   = thisString->upperLimit;	
	}
	else
	{
		start = 0;
		end = thisString->stringLength;
	}

	int j = 0;

	for (i = start; i < end; i ++)
	{
		unsigned short c = thisString->wcharBuffer[i];

		if (c > 0x7f)
			c = ' ';
  
		dest[j++] = (char)c;

		if (j >= destSize - 1)
		{
			fullyConverted = false;
			break;
		}
	}

	dest[j] = '\0';

	return fullyConverted;

}

static void print(struct UnicodeString* thisString)
{

	unsigned short* buf = thisString->wcharBuffer;

	char ascii[256];

	thisString->toASCIICString(thisString, ascii, sizeof(ascii));

	kprintf("%s", ascii);

}

static bool setSubstringRange(struct UnicodeString* thisString, unsigned int lower, unsigned int upper)
{

	if (lower < thisString->stringLength && upper <= thisString->stringLength && lower < upper)
	{
		thisString->substringActive = true;
		thisString->lowerLimit = lower;
		thisString->upperLimit = upper;
		return true;
	}

	thisString->substringActive = false;	
	
	return false;

}

static bool getSubstringRange(struct UnicodeString* thisString, unsigned int* lower, unsigned int* upper)
{

	if (thisString->substringActive)
	{
		*lower = thisString->lowerLimit;
		*upper = thisString->upperLimit;
		return true;
	}

	return false;

}

static void clearSubstringRange(struct UnicodeString* thisString)
{

	thisString->substringActive = false;

}

static bool tokenise(struct UnicodeString* thisString, char splitChar, unsigned int* start, unsigned int* end)
{
	
	if (*start >= thisString->stringLength || *start >= *end)
		return false;

	 while (thisString->wcharBuffer[*start] == (unsigned short)splitChar && *start < thisString->stringLength)
		*start = *start + 1;

	 *end = *start;

	 while (thisString->wcharBuffer[*end] != (unsigned short)splitChar && *end < thisString->stringLength)
		*end = *end + 1;

	return true;

}

static void reset(struct UnicodeString* thisString)
{
	thisString->stringLength = 0;
	thisString->substringActive = false;
}

struct UnicodeString* lib_createUnicodeString()
{

	struct UnicodeString* string = kmalloc(sizeof(struct UnicodeString), MEMORYTYPE_HIMEM); 

	if (string == NULL)
		return NULL;

	// fill in default parameters

	string->stringLength = 0;
	string->substringActive = false;

	// Allocate a default character buffer

	string->wcharBuffer = (unsigned short*)kmalloc(sizeof(unsigned short) * 256, MEMORYTYPE_HIMEM); 

	if (string->wcharBuffer == NULL)
	{
		kprintf("could not allocate memory for wcharBuffer!\n");
		kfree(string);
		return NULL;
	}

	string->wcharBufferLength = 256;

	// fill in function pointers

	string->appendASCIICString = appendASCIICString;
	string->appendUTF16String  = appendUTF16String;
	string->compare		   = compare;
	string->toASCIICString	   = toASCIICString;	
	string->print		   = print;

	string->setSubstringRange  	= setSubstringRange;
	string->getSubstringRange  	= getSubstringRange;
	string->clearSubstringRange	= clearSubstringRange;
	string->tokenise		= tokenise;

	string->reset			= reset;

	return string;
	
}

void	lib_destroyUnicodeString(struct UnicodeString** string)
{

	if ((*string)->wcharBuffer != NULL)
		kfree((*string)->wcharBuffer);

	kfree(*string);

	*string = NULL;

}
