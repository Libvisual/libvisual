#include <string.h>
#include <math.h>

#include "oinksie.h"
#include "common.h"
#include "gfx-palette.h"
#include "config.h"
#include "audio.h"

void _oink_audio_init (OinksiePrivate *priv)
{
	priv->audio.basssens = 20;
	priv->audio.tripplesens = 20;
}

/* audio priv->audio.freq analyze */
void _oink_audio_analyse (OinksiePrivate *priv)
{
	int i, j, total, mean;
	
	/* Tripple left */
	for (i = 50, total = 0; i < 190; i++)
		total += priv->audio.freq[0][i];
	
	mean = (total / 140);

	priv->audio.trippleleft = 1;
	for (i = 9; i > 1; i--)
	{
		if (mean > priv->audio.tripplesens * i)
		{
			priv->audio.trippleleft = i;
			break;
		}

	}

	/* Tripple Right */
	for (i = 50, total = 0; i < 190; i++)
		total += priv->audio.freq[1][i];
	
	mean = (total / 140);

	priv->audio.trippleright = 1;
	for (i = 9; i > 1; i--)
	{
		if (mean > priv->audio.tripplesens * i)
		{
			priv->audio.trippleright = i;
			break;
		}

	}

	/* Bass left */
	for (i = 0, total = 0; i < 35; i++)
		total += priv->audio.freq[0][i];

	mean = (total / 35);

	priv->audio.bassleft = 1;
	for (i = 9, j = 350; i > 1; i--, j -= 35)
	{
		if (mean > priv->audio.basssens * j)
		{
			priv->audio.bassleft = i;
			break;
		}
	}

	/* Bass Right */
	for (i = 0, total = 0; i < 35; i++)
		total += priv->audio.freq[1][i];

	mean = (total / 35);

	priv->audio.bassright = 1;
	for (i = 9, j = 350; i > 1; i--, j -= 35)
	{
		if (mean > priv->audio.basssens * j)
		{
			priv->audio.bassright = i;
			break;
		}
	}

	priv->audio.bass = (priv->audio.bassleft + priv->audio.bassright) / 2;
	priv->audio.tripple = (priv->audio.trippleleft + priv->audio.trippleright) / 2;
	priv->audio.highest = MAX (priv->audio.bass, priv->audio.tripple);

	if (priv->audio.bass >= 0 && priv->audio.bass <= 2)
		priv->audio.musicmood = 0;
	else if (priv->audio.bass >= 3 && priv->audio.bass <= 6)
		priv->audio.musicmood = 1;
	else if (priv->audio.bass >= 7 && priv->audio.bass <= 10)
		priv->audio.musicmood = 2;
	else
		priv->audio.musicmood = 0;

	priv->audio.beat = FALSE;
	
	if (priv->audio.bass >= 9)
		priv->audio.beat = TRUE;
}

