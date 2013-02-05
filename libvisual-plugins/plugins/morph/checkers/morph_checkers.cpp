/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2012-2013 Libvisual team
 *
 * Authors: Scott Sibley <sisibley@gmail.com>
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

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

namespace {

  struct CheckerPrivate
  {
      LV::Timer timer;
      bool      flip;
  };

  int  lv_morph_checkers_init    (VisPluginData *plugin);
  void lv_morph_checkers_cleanup (VisPluginData *plugin);
  void lv_morph_checkers_apply   (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

  unsigned int const n_tile_rows = 4;
  unsigned int const n_tile_cols = 4;

} // anonymous namespace

const VisPluginInfo *get_plugin_info ()
{
    static VisMorphPlugin morph;
    morph.apply = lv_morph_checkers_apply;
    morph.vidoptions.depth =
            VisVideoDepth (VISUAL_VIDEO_DEPTH_8BIT  |
                           VISUAL_VIDEO_DEPTH_16BIT |
                           VISUAL_VIDEO_DEPTH_24BIT |
                           VISUAL_VIDEO_DEPTH_32BIT);

    static VisPluginInfo info;

    info.type     = VISUAL_PLUGIN_TYPE_MORPH;
    info.plugname = "checkers";
    info.name     = "Checkerboard morph";
    info.author   = "Scott Sibley <sisibley@gmail.com>";
    info.version  = "0.1";
    info.about    = N_("A checkers in/out morph plugin");
    info.help     = N_("This morph plugin adds a checkerboard effect..");
    info.license  = VISUAL_PLUGIN_LICENSE_LGPL;

    info.init     = lv_morph_checkers_init;
    info.cleanup  = lv_morph_checkers_cleanup;
    info.plugin   = &morph;

    return &info;
}

namespace {

  int lv_morph_checkers_init (VisPluginData *plugin)
  {
  #if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
  #endif

      auto priv = new CheckerPrivate;
      visual_plugin_set_private (plugin, priv);

      priv->timer.start ();

      priv->flip = true;

      return TRUE;
  }

  void lv_morph_checkers_cleanup (VisPluginData *plugin)
  {
      auto priv = static_cast<CheckerPrivate*> (visual_plugin_get_private (plugin));

      delete priv;
  }

  void lv_morph_checkers_apply (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
  {
      auto priv = static_cast<CheckerPrivate*> (visual_plugin_get_private (plugin));

      if (priv->timer.elapsed ().to_msecs () > 300) {
          priv->flip = !priv->flip;
          priv->timer.reset ();
          priv->timer.start ();
      }

      dest->fill_color (LV::Color::black ());

      int dest_width  = visual_video_get_width (dest);
      int dest_height = visual_video_get_height (dest);

      int tile_width  = dest_width  / n_tile_cols;
      int tile_height = dest_height / n_tile_rows;

      for (int row = 0, y = 0; y < dest_height; row++, y += tile_height) {
          for (int col = 0, x = 0; x < dest_width; col++, x += tile_width) {
              LV::Rect region {x, y, tile_width, tile_height};

              auto src  = (row + col + priv->flip) & 1 ? src1 : src2;
              auto tile = LV::Video::create_sub (src, region);

              dest->blit (tile, x, y, false);
          }
      }
  }

} // anonymous namespace
