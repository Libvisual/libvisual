#include <stdio.h>
#include <string.h>

#include "oinksie.h"
#include "common.h"
#include "config.h"
#include "gfx-palette.h"
#include "screen.h"

void _oink_screen_init (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_size_set (priv, width, height);
}

void _oink_screen_size_set (OinksiePrivate *priv, int width, int height)
{
	priv->screen_width = width;
	priv->screen_height = height;

	priv->screen_size = priv->screen_width * priv->screen_height;

	priv->screen_halfwidth = priv->screen_width / 2;
	priv->screen_halfheight = priv->screen_height / 2;

	priv->screen_xybiggest = priv->screen_width > priv->screen_height ? priv->screen_width : priv->screen_height;
	priv->screen_xysmallest = priv->screen_width < priv->screen_height ? priv->screen_width : priv->screen_height;
}

