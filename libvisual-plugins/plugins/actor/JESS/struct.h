#ifndef _JESS_STRUCT_H
#define _JESS_STRUCT_H

#include <libvisual/libvisual.h>

struct conteur_struct  
{
	float angle;
	float angle2;
	float v_angle2;

	float dt;

	uint32_t blur_mode;


	int fps;

	uint32_t courbe;
	uint32_t mix_reprise;
	uint32_t last_flash;
	uint32_t draw_mode;
	uint32_t burn_mode;
	uint32_t k1;
	uint32_t k2;
	uint32_t k3;


	uint32_t general;
	int term_display;
	int fullscreen;
	int psy;
	int analyser;
	int freeze ;
	int freeze_mode ;


	int triplet;
};

#endif /* _JESS_STRUCT_H */
