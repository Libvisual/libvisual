/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_input.c,v 1.29 2006/01/22 13:23:37 synap Exp $
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
#include "lv_common.h"
#include "lv_plugin_registry.h"
#include "gettext.h"

namespace {

  inline LV::PluginList const&
  get_input_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_INPUT);
  }

  inline LV::PluginRef const*
  find_input_plugin (std::string const& name)
  {
      return LV::PluginRegistry::instance()->find_plugin (VISUAL_PLUGIN_TYPE_INPUT, name);
  }

} // Anonymous namespace

static int input_dtor (VisObject *object);

static VisInputPlugin *get_input_plugin (VisInput *input);

static int input_dtor (VisObject *object)
{
    VisInput *input = VISUAL_INPUT (object);

    if (input->plugin != NULL)
        visual_plugin_unload (input->plugin);

    if (input->audio != NULL)
        visual_object_unref (VISUAL_OBJECT (input->audio));

    input->plugin = NULL;
    input->audio = NULL;

    return VISUAL_OK;
}

static VisInputPlugin *get_input_plugin (VisInput *input)
{
    VisInputPlugin *inplugin;

    visual_return_val_if_fail (input != NULL, NULL);
    visual_return_val_if_fail (input->plugin != NULL, NULL);

    inplugin = VISUAL_INPUT_PLUGIN (input->plugin->info->plugin);

    return inplugin;
}

VisPluginData *visual_input_get_plugin (VisInput *input)
{
    return input->plugin;
}

const char *visual_input_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_input_plugin_list (), name);
}

const char *visual_input_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (get_input_plugin_list (), name);
}

VisInput *visual_input_new (const char *inputname)
{
    VisInput *input;
    int result;

    input = visual_mem_new0 (VisInput, 1);

    result = visual_input_init (input, inputname);
    if (result != VISUAL_OK) {
        visual_mem_free (input);
        return NULL;
    }

    /* Do the VisObject initialization */
    visual_object_set_allocated (VISUAL_OBJECT (input), TRUE);
    visual_object_ref (VISUAL_OBJECT (input));

    return input;
}

int visual_input_init (VisInput *input, const char *inputname)
{
    visual_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

    if (inputname && get_input_plugin_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, _("the plugin list is empty"));

        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_clear (VISUAL_OBJECT (input));
    visual_object_set_dtor (VISUAL_OBJECT (input), input_dtor);
    visual_object_set_allocated (VISUAL_OBJECT (input), FALSE);

    /* Reset the VisInput data */
    input->audio = visual_audio_new ();
    input->plugin = NULL;
    input->callback = NULL;

    if (inputname == NULL)
        return VISUAL_OK;

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_INPUT, inputname)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    input->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_INPUT, inputname);

    return VISUAL_OK;
}

int visual_input_realize (VisInput *input)
{
    visual_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

    if (input->plugin != NULL && input->callback == NULL)
        return visual_plugin_realize (input->plugin);

    return VISUAL_OK;
}

int visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *priv)
{
    visual_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

    input->callback = callback;
    visual_object_set_private (VISUAL_OBJECT (input), priv);

    return VISUAL_OK;
}

int visual_input_run (VisInput *input)
{
    VisInputPlugin *inplugin;

    visual_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

    if (input->callback == NULL) {
        inplugin = get_input_plugin (input);

        if (inplugin == NULL) {
            visual_log (VISUAL_LOG_ERROR, "The input plugin is not loaded correctly.");

            return -VISUAL_ERROR_INPUT_PLUGIN_NULL;
        }

        inplugin->upload (input->plugin, input->audio);
    } else
        input->callback (input, input->audio, visual_object_get_private (VISUAL_OBJECT (input)));

    visual_audio_analyze (input->audio);

    return VISUAL_OK;
}
