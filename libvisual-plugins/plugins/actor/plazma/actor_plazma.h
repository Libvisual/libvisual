#ifndef _ACTOR_PLAZMA_H
#define _ACTOR_PLAZMA_H

#include <libvisual/libvisual.h>

typedef struct {
	int	 i;
	float	*f;
} SinCos;

typedef struct {
	int16_t		 pcm_buffer[1024];
	uint16_t	 render_buffer[512];

	int		 bass;
	int		 state;
	int		 old_state;
	
	int		 width;
	int		 height;

	int		 tablex;
	int		 tabley;

	/* General */
	int		 bass_sensibility;
	int		 effect;
	int		 options;
	int		 lines;
	int		 spectrum;
	int		 use_3d;

	/* Data */
	float		 k_put;
	float		 rot_tourni;
	uint16_t	 val_maxi;
	int		 chcol0;
	double		 R[4];
	float		 compt_grille_3d;
	float		 compt_cercle_3d;

	/* Plazma tables */
	uint8_t		*tmem;
	uint8_t		*tmem1;

	/* Sin tables */
	SinCos		 cosw;
	SinCos		 sinw;

	/* Libvisual data */
	VisVideo	*video;
	uint8_t		*pixel;
	VisPalette	colors;

} PlazmaPrivate;

#endif /* _ACTOR_PLAZMA_H */
