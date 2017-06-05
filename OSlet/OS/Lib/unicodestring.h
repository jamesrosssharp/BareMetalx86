
#pragma once

#include "klib.h"

//
//	A general purpose unicode string object. Storage is UTF-16.
//	Exposes methods to convert to and from ascii, and 
//	could be extended to support UTF-8
//

struct UnicodeString
{

	unsigned short* wcharBuffer;

	unsigned int wcharBufferLength;
	unsigned int stringLength;

	unsigned int lowerLimit;
	unsigned int upperLimit;
	bool	     substringActive;

	bool (*appendASCIICString)(struct UnicodeString* thisString, const char* asciiString); 
	bool (*appendUTF16String)(struct UnicodeString* thisString, const unsigned short* utf16string, unsigned int length);

	int (*compare)(const struct UnicodeString* thisString, const struct UnicodeString* thatString);	

	// returns true if fully converted
	bool (*toASCIICString)(struct UnicodeString* thisString, char* dest, unsigned int destSize); // will fill at most destSize bytes	

	void (*print)(struct UnicodeString* thisString);

	// set / get / clear a substring... used for comparing / printing
	bool (*setSubstringRange)(struct UnicodeString* thisString, unsigned int lower, unsigned int upper);
	bool (*getSubstringRange)(struct UnicodeString* thisString, unsigned int* lower, unsigned int* upper);
	void (*clearSubstringRange)(struct UnicodeString* thisString);

	// returns substring limits of the first matching token found from start onwards
	bool (*tokenise)(struct UnicodeString* thisString, char splitChar, unsigned int* start, unsigned int* end);

	// reset length and substring (but don't reset wcharbuffer...)
	void (*reset)(struct UnicodeString* thisString);

};

struct UnicodeString* lib_createUnicodeString();
void	lib_destroyUnicodeString(struct UnicodeString** string);

