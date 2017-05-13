
#include "math.h"

unsigned int lib_math_log2(unsigned int operand)
{

	if (operand <= 0)
		return -1;

	unsigned int log2 = 0;

	while ((1 << log2) < operand)
		log2 ++;

	return log2;		

}
