/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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
#include "lv_input.h"
#include "lv_plugin_registry.h"


namespace {

  LV::PluginList const&
  get_input_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_INPUT);
  }

} // anonymous namespace

VisInput *visual_input_new (const char *name)
{
    auto self = LV::Input::load (name);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

void visual_input_ref (VisInput *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_add_ref (self);
}

void visual_input_unref (VisInput *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_release (self);
}

int visual_input_realize (VisInput *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->realize ();
}

int visual_input_run (VisInput *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->run ();
}

VisPluginData *visual_input_get_plugin (VisInput *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_plugin ();
}

VisAudio *visual_input_get_audio (VisInput *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<VisAudio *> (&self->get_audio ());
}

void visual_input_set_callback (VisInput *self, VisInputUploadCallbackFunc callback, void *user_data)
{
    using namespace std::placeholders;

    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (callback);

    auto get_audio_ptr = [=] (LV::Audio& audio) { return &audio; };

    self->set_callback (std::bind (callback, self, std::bind (get_audio_ptr, _1), user_data));
}

const char *visual_input_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_input_plugin_list (), name);
}

const char *visual_input_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (get_input_plugin_list (), name);
}
