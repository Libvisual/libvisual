#include "common.h"
#include "gfx-blur.h"
#include "gfx-misc.h"
#include "screen.h"

void _oink_gfx_blur_fade (OinksiePrivate *priv, uint8_t *buf, int fade)
{
	int i;
	uint8_t valuetab[256];

	for (i = 0; i < 256; i++)
		valuetab[i] = (i - fade) > 0 ? i - fade : 0;

	for (i = 0; i < priv->screen_size; i++)
		buf[i] = valuetab[buf[i]];
}

void _oink_gfx_blur_simple (OinksiePrivate *priv, uint8_t *buf)
{
	int i;

	for (i = 0; i < (priv->screen_size - priv->screen_width - 1); i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2] + buf[i + priv->screen_width]
			  + buf[i + 1 + (priv->screen_width)]) >> 2;
	}

	for (i = (priv->screen_size - priv->screen_width - 1); i < priv->screen_size - 2; i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2]) >> 1;
	}
}

void _oink_gfx_blur_middle (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;

	for (i = 0; i < scrsh; i++)
	{
		buf[i] = (buf[i]
		         + buf[i + priv->screen_width]
			 + buf[i + priv->screen_width + 1]
			 + buf[i + priv->screen_width - 1]) >> 2;
	}

	for (i = priv->screen_size - 1; i > scrsh; i--)
	{
		buf[i] = (buf[i] 
			 + buf[i - priv->screen_width]
			 + buf[i - priv->screen_width + 1]
			 + buf[i - priv->screen_width - 1]) >> 2;
	}
}

void _oink_gfx_blur_midstrange (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;
	
	for (i = scrsh; i > 0; i--)
	{
		buf[i] = (buf[i] 
		   	  + buf[i + priv->screen_width]
			  + buf[i + priv->screen_width + 1]
			  + buf[i + priv->screen_width - 1]) >> 2;
	}
	
	for (i = scrsh; i < priv->screen_size - 2; i++)
	{
		buf[i] = (buf[i]
			  + buf[i - priv->screen_width]
			  + buf[i - priv->screen_width + 1]
			  + buf[i - priv->screen_width - 1]) >> 2;
	}
}

