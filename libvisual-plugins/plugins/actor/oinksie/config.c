#include "common.h"
#include "audio.h"
#include "config-defines.h"
#include "gfx-misc.h"
#include "screen.h"
#include "misc.h"
#include "config.h"

/* random configs */
inline void _oink_config_random_blurmode (OinksiePrivate *priv)
{
	priv->config.blurmode = _oink_random_int (0, CONFIG_BLURMODES);
}

inline void _oink_config_random_scopemode (OinksiePrivate *priv)
{
	priv->config.scopemode = _oink_random_int (0, CONFIG_SCOPEMODES);
}

inline void _oink_config_random_backgroundmode (OinksiePrivate *priv)
{
	priv->config.backgroundmode = _oink_random_int (0, CONFIG_BACKGROUNDMODES);
}

inline void _oink_config_random_beatdots (OinksiePrivate *priv)
{
	priv->config.beatdots = _oink_random_boolean ();
}

inline void _oink_config_random_palfunky (OinksiePrivate *priv)
{
	priv->config.palfunky = _oink_random_boolean ();
}

