#include <config.h>

#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "renderer.h"
#include "compute.h"
#include "display.h"

#define wrap(a) ( a < 0 ? 0 : ( a > 255 ? 255 : a )) 

void _inf_init_renderer(InfinitePrivate *priv)
{
	int allocsize;

	allocsize = ((priv->plugwidth * priv->plugheight) + (priv->plugwidth * 2)) * NB_FCT * sizeof(t_interpol);

	priv->teff = 500;
	priv->tcol = 100;

	_inf_init_display(priv);
	_inf_generate_colors(priv);
	_inf_load_random_effect(priv, &priv->current_effect);

	priv->vector_field = (t_interpol*) visual_mem_malloc0(allocsize);
	visual_mem_set (priv->vector_field, 0, allocsize);

	_inf_generate_vector_field(priv, priv->vector_field);
}


void _inf_renderer(InfinitePrivate *priv)
{
	_inf_blur(priv, &priv->vector_field[priv->plugwidth*priv->plugheight*priv->current_effect.num_effect]);
	_inf_spectral(priv, &priv->current_effect, priv->pcm_data);
	_inf_curve(priv, &priv->current_effect);

	if (priv->t_last_color<=32) {
		_inf_change_color(priv, priv->old_color,
				priv->color,
				priv->t_last_color*8);
	}
	priv->t_last_color+=1;
	priv->t_last_effect+=1;
	if (priv->t_last_effect%priv->teff==0) {
		_inf_load_random_effect(priv, &priv->current_effect);
		priv->t_last_effect=0;
	}
	if (priv->t_last_color%priv->tcol==0) {
		priv->old_color=priv->color;
		priv->color=visual_random_context_int_range(priv->rcontext, 0, NB_PALETTES - 1);
		priv->t_last_color=0;
	}
}

void _inf_close_renderer(InfinitePrivate *priv)
{
	visual_mem_free(priv->surface1);
	visual_mem_free(priv->surface2);
	visual_mem_free(priv->vector_field);
}

