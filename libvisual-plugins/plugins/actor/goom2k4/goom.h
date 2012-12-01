#ifndef _GOOMCORE_H
#define _GOOMCORE_H

#include "goom_config.h"
#include "goom_plugin_info.h"
#include "goomsl.h"

#define NB_FX 10

PluginInfo *goom_init (uint32_t resx, uint32_t resy);
void goom_set_resolution (PluginInfo *goomInfo, uint32_t resx, uint32_t resy);

/*
 * forceMode == 0 : do nothing
 * forceMode == -1 : lock the FX
 * forceMode == 1..NB_FX : force a switch to FX n# forceMode
 *
 * songTitle = pointer to the title of the song...
 *      - NULL if it is not the start of the song
 *      - only have a value at the start of the song
 */
uint32_t *goom_update (PluginInfo *goomInfo, int16_t data[2][512], int forceMode, float fps,
                      const char *songTitle, const char *message);

/* returns 0 if the buffer wasn't accepted */
int goom_set_screenbuffer(PluginInfo *goomInfo, void *buffer);

void goom_close (PluginInfo *goomInfo);

#endif
