//
//
//	mathtest: test the kernel math library
//
//
//

#include "../../../OS/Lib/math.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char** argv)
{

	const int numbers[] = {1, 2, 16, 256, 0, -1};

	for (int i = 0; i < COUNTOF(numbers); i ++)
	{
		int v = numbers[i];
		cout << "Log2 of " << v << " is " << lib_math_log2(v) << endl;
	}

}
