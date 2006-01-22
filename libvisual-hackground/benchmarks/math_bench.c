#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#include "elapsed.h"

#define SIZE	1024 * 1024 * 32
#define TIMES	4

VisTimer timer;
long long res1;
long long res2;
uint32_t lo, hi;

void hit () {
	visual_timer_tsc_get (&lo, &hi);

	res1 = res2;

	res2 = ((long long) hi << 32) | lo;

	printf ("Cycles since last hit: %d\n", res2 - res1);
}

int main (int argc, char **argv)
{
	VisRectangle rect;
	float *f, *real, *imag;
	int *ints;
	float mul = 2;
	int i;

	hit (); hit ();

	f = malloc (SIZE * sizeof (float));
	real = malloc (SIZE * sizeof (float));
	imag = malloc (SIZE * sizeof (float));
	ints = malloc (SIZE * sizeof (int));

	hit ();
	visual_time_usleep (VISUAL_USEC_PER_SEC);
	hit ();

	for (i = 0; i < SIZE; i++)
		f[i] = (1.0 / SIZE) * i;

	visual_cpu_initialize ();


        visual_timer_init (&timer);

	/* Const multiply */
	visual_timer_start (&timer);

	printf ("\n");

	hit ();
	for (i = 0; i < TIMES; i++)
		visual_math_vectorized_multiplier_floats_const_float (f, f, SIZE, mul);

	hit ();

	visual_timer_stop (&timer);

	show_elapsed ("Timer done: ", &timer);

	printf ("vector multiplied %d :: %d times {}> %d\n", SIZE, TIMES, SIZE * TIMES);

	/* Floats to ints */
	visual_timer_start (&timer);

	printf ("\n");
	hit ();
	for (i = 0; i < TIMES; i++)
		visual_math_vectorized_floats_to_int32s (ints, f, SIZE);

	hit ();

	visual_timer_stop (&timer);

	show_elapsed ("Timer done: ", &timer);

	printf ("Floats to ints %d :: %d times {}> %d\n", SIZE, TIMES, SIZE * TIMES);

	/* Ints to float */
	visual_timer_start (&timer);

	printf ("\n");
	hit ();
	for (i = 0; i < TIMES; i++)
		visual_math_vectorized_int32s_to_floats (f, ints, SIZE);

	hit ();

	visual_timer_stop (&timer);

	show_elapsed ("Timer done: ", &timer);

	printf ("Ints to float %d :: %d times {}> %d\n", SIZE, TIMES, SIZE * TIMES);

	/* complex to normal scale */
	visual_timer_start (&timer);

	printf ("\n");
	hit ();
	for (i = 0; i < TIMES; i++)
		visual_math_vectorized_complex_to_norm_scale (f, real, imag, SIZE, 1);

	hit ();

	visual_timer_stop (&timer);

	show_elapsed ("Timer done: ", &timer);

	printf ("Complex to normal scale %d :: %d times {}> %d\n", SIZE, TIMES, SIZE * TIMES);

}

