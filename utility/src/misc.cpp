#include "utility/misc.h"
#include <stdlib.h>

float randrange(float min, float max)
{
	return (((float)rand() * (max - min) / RAND_MAX) + min);
}

float clamp(float min, float max, float flt)
{
	if(flt > max) 
	{
		return max;
	}
	else if(flt < min)
	{
		return min;
	}
	else{
		return flt;
	}
}