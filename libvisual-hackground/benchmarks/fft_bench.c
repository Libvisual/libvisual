#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	500
#define FFT_OUT 4096

int main (int argc, char **argv)
{
	VisFFT *fft;
	int i;
	float temp[FFT_OUT];
	float audio[512];

	visual_cpu_initialize ();
	visual_fft_initialize ();
//	visual_init (&argc, &argv);

	fft = visual_fft_new (512, FFT_OUT);

	for (i = 0; i < TIMES; i++)
		visual_fft_perform (fft, audio, temp);

	printf ("FFT analyze %d times, size: %d\n", TIMES, FFT_OUT);
}

