#include <math.h>

#include "common.h"
#include "table.h"

void _oink_table_init ()
{
	float adder;
	int i;

	adder = 0;
	for (i = 0; i < OINK_TABLE_NORMAL_SIZE; i++)
	{
		_oink_table_sin[i] = sin (adder);
		_oink_table_cos[i] = cos (adder);
		
		adder += ((PI * 2) / OINK_TABLE_NORMAL_SIZE);
	}

	adder = 0;
	for (i = 0; i < OINK_TABLE_LARGE_SIZE; i++)
	{
		_oink_table_sinlarge[i] = sin (adder);
		_oink_table_coslarge[i] = cos (adder);
		
		adder += ((PI * 2) / OINK_TABLE_LARGE_SIZE);
	}
}

