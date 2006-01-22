#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	500
#define FFT_OUT 4096

int main (int argc, char **argv)
{
	VisDFT *fft;
	int i;
	float temp[FFT_OUT];
	float audio[512];

	visual_cpu_initialize ();
	visual_fourier_initialize ();
//	visual_init (&argc, &argv);

	fft = visual_dft_new (512, FFT_OUT);

	for (i = 0; i < TIMES; i++)
		visual_dft_perform (fft, audio, temp);

	printf ("DFT analyze %d times, size: %d\n", TIMES, FFT_OUT);
}

