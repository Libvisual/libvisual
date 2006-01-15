#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#include "elapsed.h"

#define SIZE	1024 * 1024 * 32
#define TIMES	4

VisTimer timer;


int main (int argc, char **argv)
{
	VisRectangle rect;
	float *f;
	float mul = 2;
	int i;

	f = malloc (SIZE * sizeof (float));

	for (i = 0; i < SIZE; i++)
		f[i] = (1.0 / SIZE) * i;

	visual_cpu_initialize ();


        visual_timer_init (&timer);

	visual_timer_start (&timer);

	visual_cpu_set_3dnow (FALSE);

	for (i = 0; i < TIMES; i++)
		visual_math_vectorized_multiplier_floats_const_float (f, SIZE, mul);

	visual_timer_stop (&timer);

	show_elapsed ("\tTimer done: ", &timer);


	printf ("vector multiplied %d :: %d times {}> %d\n", SIZE, TIMES, SIZE * TIMES);
}

