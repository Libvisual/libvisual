#include <string.h>
#include <gtk/gtk.h>
#include <xmms/configfile.h>

#include "config.h"
#include "config_gui.h"

#define CONFIG_DEFAULT_ICON (PACKAGE_DATADIR "/libvisual-xmms-vis.bmp")

static const Options default_options = { NULL, NULL, 320, 200, 30, 24, FALSE };
static Options options = { NULL, NULL, -1, -1, -1, -1, FALSE};
static ConfigWin *config_win = NULL;

static gboolean fullscreen;

/* Callbacks */
static void on_button_ok_clicked (GtkButton *button, gpointer user_data);
static void on_button_cancel_clicked (GtkButton *button, gpointer user_data);
static void on_checkbutton_fullscreen_toggled (GtkToggleButton *togglebutton, gpointer user_data);
static void on_checkbutton_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data);
static void on_spinbutton_fps_changed (GtkEditable *editable, gpointer user_data);
static void on_checkbutton_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data);
static gboolean on_pixmap_icon_button_press_event (GtkWidget *widget,
						GdkEventButton *event,
						gpointer user_data);

Options *lv_xmms_config_open ()
{
	options.last_plugin = g_malloc (OPTIONS_MAX_NAME_LEN);
	options.icon_file = g_malloc (OPTIONS_MAX_ICON_PATH_LEN);

	return &options;
}

int lv_xmms_config_close ()
{
	if (options.last_plugin != NULL)
		g_free (options.last_plugin);
	if (options.icon_file != NULL)
		g_free (options.icon_file);

	return 0;
}

int lv_xmms_config_load_prefs ()
{
	gchar *vstr;
	ConfigFile *f;
	gboolean errors;
	gboolean must_create_entry;
	gboolean must_update;

	if ((f = xmms_cfg_open_default_file ()) == NULL)
		return -1;

	errors = FALSE;
	must_create_entry = FALSE;
	must_update = FALSE;
	if (xmms_cfg_read_string (f, "libvisual_xmms", "version", &vstr)) {
		if (strcmp (vstr, VERSION) == 0) {
			errors = FALSE;
			if (!xmms_cfg_read_string (f, "libvisual_xmms", "last_plugin", &options.last_plugin)
				|| (strlen (options.last_plugin) <= 0)) {
				strcpy (options.last_plugin, "infinite");
				errors = TRUE;
			}
			if (!xmms_cfg_read_string (f, "libvisual_xmms", "icon", &options.icon_file)
				|| (strlen (options.icon_file) <= 0)) {
				strcpy (options.icon_file, CONFIG_DEFAULT_ICON);
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "width", &options.width)) {
				options.width = default_options.width;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "height", &options.height)) {
				options.height = default_options.height;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "fps", &options.fps)) {
				options.fps = default_options.fps;
				errors = TRUE;
			}
			if (!xmms_cfg_read_int (f, "libvisual_xmms", "color_depth", &options.depth)) {
				options.depth = default_options.depth;
				errors = TRUE;
			}
			if (!xmms_cfg_read_boolean (f, "libvisual_xmms", "fullscreen", &options.fullscreen)) {
				options.fullscreen = default_options.fullscreen;
				errors = TRUE;
			}
		} else {
			must_update = TRUE;
		}
	} else {
		must_create_entry = TRUE;
	}
	
	if (must_update || must_create_entry) {
		strcpy (options.last_plugin, "infinite");
		strcpy (options.icon_file, CONFIG_DEFAULT_ICON);
		options.width = default_options.width;
		options.height = default_options.height;
		options.depth = default_options.depth;
		options.fps = default_options.fps;
		options.fullscreen = default_options.fullscreen;
	}

	xmms_cfg_free (f);

	/*
	 * Set our local copy
	 */
	fullscreen = options.fullscreen;

	if (errors) {
		g_message ("LibVisual XMMS plugin: config file contain errors, fixing...");
		lv_xmms_config_save_prefs ();
	} else if (must_update) {
		g_message ("LibVisual XMMS plugin: config file is from old version, updating...");
		lv_xmms_config_save_prefs ();
	} else if (must_create_entry) {
		g_message ("LibVisual XMMS plugin: adding entry to config file...");
		lv_xmms_config_save_prefs ();
	}

	return 0;
}


int lv_xmms_config_save_prefs ()
{
	ConfigFile *f;

	if((f = xmms_cfg_open_default_file ()) == NULL)
		f = xmms_cfg_new ();
	if (f == NULL)
		return -1;

	xmms_cfg_write_string (f, "libvisual_xmms", "version", VERSION);

	if (options.last_plugin != NULL && (strlen(options.last_plugin) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", options.last_plugin);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", "infinite");

	if (options.icon_file != NULL && (strlen(options.icon_file) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "icon", options.icon_file);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "icon", CONFIG_DEFAULT_ICON);

	xmms_cfg_write_int (f, "libvisual_xmms", "width", options.width);
	xmms_cfg_write_int (f, "libvisual_xmms", "height", options.height);
	xmms_cfg_write_int (f, "libvisual_xmms", "color_depth", options.depth);
	xmms_cfg_write_int (f, "libvisual_xmms", "fps", options.fps);
	xmms_cfg_write_boolean (f, "libvisual_xmms", "fullscreen", options.fullscreen);

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);

	return 0;
}

void lv_xmms_config_toggle_fullscreen (void)
{
	fullscreen = !fullscreen;
	options.fullscreen = !options.fullscreen;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen), fullscreen);
}

void lv_xmms_config_window ()
{
	if (config_win != NULL) {
		gtk_widget_show (config_win->window_main);
		return;
	}

	config_win = lv_xmms_config_gui_new ();

	gtk_signal_connect (GTK_OBJECT (config_win->checkbutton_fullscreen), "toggled",
                      GTK_SIGNAL_FUNC (on_checkbutton_fullscreen_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->checkbutton_opengl), "toggled",
                      GTK_SIGNAL_FUNC (on_checkbutton_opengl_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->spinbutton_fps), "changed",
                      GTK_SIGNAL_FUNC (on_spinbutton_fps_changed),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->pixmap_icon), "button_press_event",
                      GTK_SIGNAL_FUNC (on_pixmap_icon_button_press_event),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->button_ok), "clicked",
                      GTK_SIGNAL_FUNC (on_button_ok_clicked),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->button_cancel), "clicked",
                      GTK_SIGNAL_FUNC (on_button_cancel_clicked),
                      NULL);

	gtk_widget_grab_default (config_win->button_cancel);

	gtk_widget_show (config_win->window_main);
}

/*static void on_window_main_destroy (GtkObject *object, gpointer user_data)
{

}*/


static void on_checkbutton_fullscreen_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	fullscreen = !fullscreen;
}


static void on_checkbutton_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{

}


static void on_spinbutton_fps_changed (GtkEditable *editable, gpointer user_data)
{

}


static gboolean on_pixmap_icon_button_press_event (GtkWidget *widget,
						GdkEventButton *event,
						gpointer user_data)
{

  return FALSE;
}


static void on_button_ok_clicked (GtkButton *button, gpointer user_data)
{
	options.fullscreen = fullscreen;

	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET(button)));
}


static void on_button_cancel_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET(button)));
}


