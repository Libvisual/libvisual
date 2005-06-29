#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define SIZE	1024 * 32
#define TIMES	500

int main (int argc, char **argv)
{
	VisRectangle rect;
	float fx[SIZE];
	float fy[SIZE];
	int x[SIZE];
	int y[SIZE];
	int i;

	visual_rectangle_set (&rect, 0, 0, 500, 500);

	visual_init (&argc, &argv);

	for (i = 0; i < TIMES; i++) {
		visual_rectangle_denormalise_many_values_neg (&rect, fx, fy, x, y, SIZE);
	}

	printf ("denormalise %d :: %d times\n", SIZE, TIMES);
}

