#ifndef _ACTOR_BUMPSCOPE_H
#define _ACTOR_BUMPSCOPE_H

#include <libvisual/libvisual.h>

typedef struct {
	int		 height;
	int		 width;

	VisPalette	 pal;

	float		 intense1[256];
	float		 intense2[256];

	int		 phongres;
	uint8_t		*phongdat;

	uint8_t		*rgb_buf;
	uint8_t		*rgb_buf2;

	VisVideo	*video;

	float		 h;
	float		 s;
	float		 v;

	/* Configs */
	VisColor	 color;
	int		 color_cycle;
	int		 moving_light;
	int		 diamond;
	int		 light_x;
	int		 light_y;

} BumpscopePrivate;

#endif /* _ACTOR_BUMPSCOPE_H */
