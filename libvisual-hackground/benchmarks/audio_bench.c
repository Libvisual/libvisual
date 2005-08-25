#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	500

int main (int argc, char **argv)
{
	VisAudio *audio;
	int i;
	
	visual_init (&argc, &argv);

	audio = visual_audio_new ();
	
	for (i = 0; i < TIMES; i++)
		visual_audio_analyze (audio);

	printf ("audio analyze %d times\n", TIMES);
}

