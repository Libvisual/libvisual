#ifndef __LV_XMMS_CONFIG__
#define __LV_XMMS_CONFIG__

#include <glib.h>
#include <libvisual/libvisual.h>

#define OPTIONS_MAX_NAME_LEN 256
#define OPTIONS_MAX_ICON_PATH_LEN 256

typedef struct {
	gchar *last_plugin;	/**< Name of the last plugin runned,
				  with length < OPTIONS_MAX_NAME_LEN. */
	gchar *icon_file;	/**< Absolute path of the icon file,
				  with length < OPTIONS_MAX_ICON_PATH_LEN. */
	int width;		/**< Width in pixels. */
	int height;		/**< Height in pixels. */
	int fps;		/**< Maximum frames per second. */
	VisVideoDepth depth;	/**< Color depth. */
	gboolean fullscreen;	/**< Say if we are in fullscreen or not. */
} Options;


int lv_xmms_config_load_prefs (Options *options);

int lv_xmms_config_save_prefs (const Options *options);


#endif /* __LV_XMMS_CONFIG__ */

