#include "utils.h"

//Counts the number of 1's in byte
int countOnes(char byte)
{
	int counter = 0;
	int i;
	for(i = 0; i < 8; i++)
	{
		printf("byte %d\n", byte);
		if((byte & 1) == 1)
			counter++;
		byte >>= 1;
	}

	return counter;
}
