#ifndef _PLAZMA_H
#define _PLAZMA_H

#include "actor_plazma.h"

void _plazma_init (PlazmaPrivate *priv);
void _plazma_run (PlazmaPrivate *priv);
void _plazma_cleanup (PlazmaPrivate *priv);
void _plazma_change_effect (PlazmaPrivate *priv);

#endif /* _PLAZMA_H */
