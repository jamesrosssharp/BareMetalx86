
#include "math.h"

int lib_math_log2(int operand)
{

	if (operand <= 0)
		return -1;

	int log2 = 0;

	while ((1 << log2) < operand)
		log2 ++;

	return log2;		

}
