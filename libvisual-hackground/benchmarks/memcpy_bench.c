#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	2000
#define BYTES	1048576

int main (int argc, char **argv)
{
	uint32_t *buf1, *mem1;
	uint32_t *buf2, *mem2;
	int i = TIMES;

	visual_init (&argc, &argv);

	buf1 = visual_mem_malloc0 (BYTES);
	buf2 = visual_mem_malloc0 (BYTES);
	
	while (i--)
		visual_mem_copy (buf1, buf2, BYTES);

	printf ("Visual mem copy %d times %d bytes: %d.\n", TIMES, BYTES, TIMES * BYTES);

	return 0;
}

