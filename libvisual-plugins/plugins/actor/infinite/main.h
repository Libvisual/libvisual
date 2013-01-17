#ifndef _INF_MAIN_H
#define _INF_MAIN_H

#include <libvisual/libvisual.h>

#define NB_PALETTES 5

struct infinite_col {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

typedef struct t_coord {
	int x,y;
} t_coord;

typedef struct t_complex {
	float x,y;
} t_complex;

typedef struct t_interpol {
	uint32_t coord ;  //coordinates of the top left pixel.
	uint32_t weight; //32 bits = 4*8= weights of the four corners
} t_interpol;

typedef struct t_effect {
	int num_effect;
	int x_curve;
	int curve_color;
	int curve_amplitude;
	int spectral_amplitude;
	int spectral_color;
	int mode_spectre;
	int spectral_shift;
} t_effect;

typedef struct {
	float pcm_data[2][512];
	int plugwidth;
	int plugheight;
	VisPalette *pal;

	/* Random context for this plugin */
	VisRandomContext *rcontext;

	uint8_t *surface1;
	uint8_t *surface2;

	int teff;
	int tcol;

	struct infinite_col color_table[NB_PALETTES][256];

	int old_color;
	int color;
	int t_last_color;
	int t_last_effect;

	t_effect current_effect;
	t_interpol *vector_field;
} InfinitePrivate;

#endif /* _INF_MAIN_H */
