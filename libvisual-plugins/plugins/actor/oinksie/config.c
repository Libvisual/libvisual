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
	priv->config.blurmode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_BLURMODES);
}

inline void _oink_config_random_scopemode (OinksiePrivate *priv)
{
	priv->config.scopemode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_SCOPEMODES);
}

inline void _oink_config_random_backgroundmode (OinksiePrivate *priv)
{
	priv->config.backgroundmode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_BACKGROUNDMODES);
}

inline void _oink_config_random_beatdots (OinksiePrivate *priv)
{
	priv->config.beatdots = visual_random_context_int_range (priv->rcontext, 0, 1);
}

inline void _oink_config_random_palfunky (OinksiePrivate *priv)
{
	priv->config.palfunky = visual_random_context_int_range (priv->rcontext, 0, 1);
}

