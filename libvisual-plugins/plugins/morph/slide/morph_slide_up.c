/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "common.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int lv_morph_slide_init_up (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
    static VisPluginInfo info = {
        .type     = VISUAL_PLUGIN_TYPE_MORPH,

        .plugname = "slide_up",
        .name     = "Slide up morph",
        .author   = "Dennis Smit <ds@nerds-incorporated.org>",
        .version  = "0.1",
        .about    = N_("A slide in/out morph plugin"),
        .help     = N_("This morph plugin morphs between two video sources by sliding one in and the other out"),
        .license  = VISUAL_PLUGIN_LICENSE_LGPL,

        .init    = lv_morph_slide_init_up,
        .cleanup = lv_morph_slide_cleanup,
        .plugin  = &morph
    };

    return &info;
}

static int lv_morph_slide_init_up (VisPluginData *plugin)
{
    return lv_morph_slide_init (plugin, SLIDE_UP);
}
