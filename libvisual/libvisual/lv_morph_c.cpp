/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_morph.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"

namespace {

  LV::PluginList const&
  get_morph_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_MORPH);
  }

} // anonymous namespace

const char *visual_morph_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_morph_plugin_list (), name);
}

const char *visual_morph_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (get_morph_plugin_list (), name);
}

VisPluginData *visual_morph_get_plugin (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, nullptr);

    return morph->get_plugin ();
}

VisMorph *visual_morph_new (const char *name)
{
    auto self = LV::Morph::load (name);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

int visual_morph_realize (VisMorph *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->realize ();
}

VisVideoDepth visual_morph_get_supported_depths (VisMorph *self)
{
    visual_return_val_if_fail (self != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    return self->get_supported_depths ();
}

VisVideoAttrOptions *visual_morph_get_video_attribute_options (VisMorph *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<VisVideoAttrOptions*> (self->get_video_attribute_options ());
}

void visual_morph_set_video (VisMorph *self, VisVideo *video)
{
    visual_return_if_fail (self != nullptr);

    self->set_video (video);
}

void visual_morph_set_time (VisMorph *self, VisTime *time)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (time != nullptr);

    self->set_time (*time);
}

void visual_morph_set_progress (VisMorph *self, float progress)
{
    visual_return_if_fail (self != nullptr);

    self->set_progress (progress);
}

VisPalette *visual_morph_get_palette (VisMorph *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<LV::Palette*> (self->get_palette ());
}

int visual_morph_is_done (VisMorph *self)
{
    visual_return_val_if_fail (self != nullptr, TRUE);

    return self->is_done ();
}

int visual_morph_run (VisMorph *self, VisAudio *audio, VisVideo *src1, VisVideo *src2)
{
    visual_return_val_if_fail (self  != nullptr, FALSE);
    visual_return_val_if_fail (audio != nullptr, FALSE);
    visual_return_val_if_fail (src1  != nullptr, FALSE);
    visual_return_val_if_fail (src2  != nullptr, FALSE);

    return self->run (*audio, src1, src2);
}
