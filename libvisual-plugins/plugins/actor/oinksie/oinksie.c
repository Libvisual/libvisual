#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "audio.h"
#include "table.h"
#include "screen.h"
#include "gfx-palette.h"
#include "scene.h"
#include "oinksie.h"

void oinksie_init (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_init (priv, width, height);
	_oink_audio_init (priv);
	_oink_table_init ();
	_oink_gfx_palette_init (priv);
	_oink_scene_randomize (priv);
}

void oinksie_quit (OinksiePrivate *priv)
{

}

void oinksie_sample (OinksiePrivate *priv)
{
	_oink_audio_analyse (priv);
}

void oinksie_render (OinksiePrivate *priv)
{
	_oink_scene_render (priv);
}

void oinksie_palette_change (OinksiePrivate *priv, uint8_t funky)
{
	_oink_gfx_palette_build (priv, funky);
}

int oinksie_size_set (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_size_set (priv, width, height);

	return 0;
}

VisPalette *oinksie_palette_get (OinksiePrivate *priv)
{
	/* return values for transformation */
	if (priv->pal_data.pal_new == 1)
	{
		_oink_gfx_palette_transform (priv);

		return &priv->pal_old;
	}
	else
	{
		return &priv->pal_cur;
	}
}

