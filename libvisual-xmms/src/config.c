#include <xmms/configfile.h>
#include <gtk/gtk.h>

#include "config.h"


#define CONFIG_DEFAULT_ICON (PACKAGE_DATADIR "/oinksie-xmms-vis.bmp")


static const Options default_options = { NULL, NULL, 320, 200, 30, 24, FALSE };


/*
int lv_xmms_config_open ()
{
}

int lv_xmms_config_close ()
{
	if (options->last_plugin != NULL)
		g_free (options);
	options->icon_file = g_malloc (OPTIONS_MAX_ICON_PATH_LEN);	
}*/

int lv_xmms_config_load_prefs (Options *options)
{
	gchar *vstr;
	ConfigFile *f;
	gboolean errors;
	gboolean must_create_entry;
	gboolean must_update;

	visual_log_return_val_if_fail (options != NULL, -1);

	if ((f = xmms_cfg_open_default_file ()) == NULL)
		return -1;

	options->last_plugin = g_malloc (OPTIONS_MAX_NAME_LEN);
	options->icon_file = g_malloc (OPTIONS_MAX_ICON_PATH_LEN);	

	errors = FALSE;
	must_create_entry = FALSE;
	must_update = FALSE;
	if (xmms_cfg_read_string (f, "libvisual_xmms", "version", &vstr)) {
		if (strcmp (vstr, VERSION) == 0) {
			errors = FALSE;
			if (!xmms_cfg_read_string (f, "libvisual_xmms", "last_plugin", &options->last_plugin)
				|| (strlen (options->last_plugin) <= 0)) {
				strcpy (options->last_plugin, "infinite");
				errors = TRUE;
			}
			if (!xmms_cfg_read_string (f, "libvisual_xmms", "icon", &options->icon_file)
				|| (strlen (options->icon_file) <= 0)) {
				strcpy (options->icon_file, CONFIG_DEFAULT_ICON);
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "width", &options->width)) {
				options->width = default_options.width;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "height", &options->height)) {
				options->height = default_options.height;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "fps", &options->fps)) {
				options->fps = default_options.fps;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "color_depth", &options->depth)) {
				options->depth = default_options.depth;
				errors = TRUE;
			}
			if (!xmms_cfg_read_boolean (f, "libvisual_xmms", "fullscreen", &options->fullscreen)) {
				options->fullscreen = default_options.fullscreen;
				errors = TRUE;
			}
		} else {
			must_update = TRUE;
			strcpy (options->last_plugin, "infinite");
			strcpy (options->icon_file, CONFIG_DEFAULT_ICON);
			options->width = default_options.width;
			options->height = default_options.height;
			options->depth = default_options.depth;
			options->fullscreen = default_options.fullscreen;
		}
	} else {
		must_create_entry = TRUE;
		strcpy (options->last_plugin, "infinite");
		strcpy (options->icon_file, CONFIG_DEFAULT_ICON);
		options->width = default_options.width;
		options->height = default_options.height;
		options->depth = default_options.depth;
		options->fullscreen = default_options.fullscreen;
	}

	xmms_cfg_free (f);

	if (errors) {
		g_message ("LibVisual XMMS plugin: config file contain errors, fixing...");
		lv_xmms_config_save_prefs (options);
	} else if (must_update) {
		g_message ("LibVisual XMMS plugin: config file is from old version, updating...");
		lv_xmms_config_save_prefs (options);
	} else if (must_create_entry) {
		g_message ("LibVisual XMMS plugin: adding entry to config file...");
		lv_xmms_config_save_prefs (options);
	}

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

	if (options->last_plugin != NULL && (strlen(options->last_plugin) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", options->last_plugin);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", "infinite");

	if (options->icon_file != NULL && (strlen(options->icon_file) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "icon", options->icon_file);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "icon", CONFIG_DEFAULT_ICON);

	xmms_cfg_write_int (f, "libvisual_xmms", "width", options->width);
	xmms_cfg_write_int (f, "libvisual_xmms", "height", options->height);
	xmms_cfg_write_int (f, "libvisual_xmms", "color_depth", options->depth);
	xmms_cfg_write_int (f, "libvisual_xmms", "fps", options->fps);
	xmms_cfg_write_boolean (f, "libvisual_xmms", "fullscreen", options->fullscreen);

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);

	return 0;
}

