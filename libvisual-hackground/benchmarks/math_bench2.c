#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	VisRectangle rect;
	float f[16];
	float mul = 2;
	int i;

	for (i = 0; i < 16; i++)
		f[i] = i;

	visual_cpu_initialize ();

	visual_math_vectorized_multiplier_float_const_float (f, 16, 2);

	for (i = 0; i < 16; i++)
		printf ("%f\n", f[i]);

	return 0;
}

