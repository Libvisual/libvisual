#include <xmms/configfile.h>
#include <gtk/gtk.h>

#include "config.h"


static const Options default_options = { NULL, 320, 200, 30, 24, FALSE };


int lv_xmms_config_load_prefs (Options *options)
{
	gchar *vstr;
	ConfigFile *f;

	visual_log_return_val_if_fail (options != NULL, -1);

	if ((f = xmms_cfg_open_default_file ()) == NULL)
		return -1;

	/*
	 * WARNING This must be synchronized with main module.
	 */
	options->last_plugin = g_malloc (OPTIONS_MAX_NAME_LEN);

	if (xmms_cfg_read_string (f, "libvisual_xmms", "version", &vstr)) {
		if (strcmp (vstr, VERSION) == 0) {
			xmms_cfg_read_string (f, "libvisual_xmms", "last_plugin", &options->last_plugin);
			xmms_cfg_read_int (f, "libvisual_xmms", "width", &options->width);
			xmms_cfg_read_int (f, "libvisual_xmms", "height", &options->height);
			xmms_cfg_read_int (f, "libvisual_xmms", "fps", &options->fps);
			xmms_cfg_read_int (f, "libvisual_xmms", "color_depth", &options->depth);
			xmms_cfg_read_boolean (f, "libvisual_xmms", "fullscreen", &options->fullscreen);
		} else {
			/*
        	         * Update to new version.
	                 */
			lv_xmms_config_save_prefs (&default_options);

			strcpy (options->last_plugin, "infinite");
			options->width = default_options.width;
			options->height = default_options.height;
			options->depth = default_options.depth;
			options->fullscreen = default_options.fullscreen;
		}
	} else {
		/*
                 * Out entry doesn't exists on the config file. 
                 */
		lv_xmms_config_save_prefs (&default_options);

		strcpy (options->last_plugin, "infinite");
		options->width = default_options.width;
		options->height = default_options.height;
		options->depth = default_options.depth;
		options->fullscreen = default_options.fullscreen;
	}

	xmms_cfg_free (f);

	return 0;
}


int lv_xmms_config_save_prefs (const Options *options)
{
	ConfigFile *f;

	visual_log_return_val_if_fail (options != NULL, -1);

	if((f = xmms_cfg_open_default_file ()) == NULL)
		f = xmms_cfg_new ();
	if (f == NULL)
		return -1;

	xmms_cfg_write_string (f, "libvisual_xmms", "version", VERSION);
	if (options->last_plugin != NULL)
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", options->last_plugin);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", "infinite");
	xmms_cfg_write_int (f, "libvisual_xmms", "width", options->width);
	xmms_cfg_write_int (f, "libvisual_xmms", "height", options->height);
	xmms_cfg_write_int (f, "libvisual_xmms", "color_depth", options->depth);
	xmms_cfg_write_int (f, "libvisual_xmms", "fps", options->fps);
	xmms_cfg_write_boolean (f, "libvisual_xmms", "fullscreen", options->fullscreen);

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);

	return 0;
}

