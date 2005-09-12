#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#include "elapsed.h"

#define SIZE	300000

VisRandomContext r;

char *gen_key (char *key)
{
	key[0] = 'a' + visual_random_context_int_range (&r, 0, 25);
	key[1] = 'a' + visual_random_context_int_range (&r, 0, 25);
	key[2] = 'a' + visual_random_context_int_range (&r, 0, 25);
	key[3] = 'a' + visual_random_context_int_range (&r, 0, 25);
	key[4] = '\0';

	return key;
}

int main (int argc, char **argv)
{
	VisTimer timer;
	VisCache cache;
	int i;
	char key[5];

	visual_timer_init (&timer);
	visual_timer_start (&timer);

	show_elapsed ("Before init:      ", &timer);

	visual_init (&argc, &argv);

	visual_random_context_init (&r, 424242);

	show_elapsed ("After init:       ", &timer);

	visual_cache_init (&cache, NULL, SIZE, NULL);

	show_elapsed ("After cache init: ", &timer);

	for (i = 0; i < SIZE; i++) {
		visual_cache_put (&cache, gen_key (key), &i);
	}

	printf ("IN CACHE: %d\n", visual_cache_get_size (&cache));

	show_elapsed ("After test add:   ", &timer);

	printf ("Visual cache fill, added %d elements.\n", SIZE);

	visual_quit ();

	return 0;
}

