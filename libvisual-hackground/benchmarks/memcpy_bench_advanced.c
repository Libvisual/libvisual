#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#include "elapsed.h"

#define TIMES	1000
#define BYTES	1048576

VisTimer timer;
uint32_t *buf1, *mem1;
uint32_t *buf2, *mem2;
int size;
int i;

void test()
{
	size = BYTES;
	while (size > 1) {
		visual_timer_start (&timer);

		for (i = 0; i < TIMES; i++)
			visual_mem_copy (buf1, buf2, size);

		show_elapsed ("\tTimer done: ", &timer);
		printf ("\tTimes %d Size %d\n", TIMES, size);

		size /= 2;
	}
}

int main (int argc, char **argv)
{

	visual_init (&argc, &argv);

	visual_timer_init (&timer);

	buf1 = visual_mem_malloc0 (BYTES);
	buf2 = visual_mem_malloc0 (BYTES);

	memset (buf1, 42, BYTES);
	memset (buf2, 21, BYTES);

	/* C version */
	visual_cpu_set_mmx (FALSE);
	visual_cpu_set_mmx2 (FALSE);
	visual_cpu_set_sse (FALSE);
	visual_cpu_set_sse2 (FALSE);
	visual_mem_initialize ();

	printf ("C:\n");
	test ();

	/* MMX version */
	visual_cpu_set_mmx (TRUE);
	visual_cpu_set_mmx2 (FALSE);
	visual_cpu_set_sse (FALSE);
	visual_cpu_set_sse2 (FALSE);
	visual_mem_initialize ();

	printf ("MMX:\n");
	test ();

	/* MMX2 version */
	visual_cpu_set_mmx (FALSE);
	visual_cpu_set_mmx2 (TRUE);
	visual_cpu_set_sse (FALSE);
	visual_cpu_set_sse2 (FALSE);
	visual_mem_initialize ();

	printf ("MMX2:\n");
	test ();

	/* SSE version */
	visual_cpu_set_mmx (FALSE);
	visual_cpu_set_mmx2 (FALSE);
	visual_cpu_set_sse (TRUE);
	visual_cpu_set_sse2 (FALSE);
	visual_mem_initialize ();

	printf ("SSE (warning, disabled in LV):\n");
	test ();

	return 0;
}

