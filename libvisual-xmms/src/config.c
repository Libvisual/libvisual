#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <xmms/configfile.h>

#include "config.h"
#include "config_gui.h"
#include "gettext.h"

#define CONFIG_DEFAULT_ICON (PACKAGE_DATADIR "/libvisual-xmms-vis.bmp")

static const Options default_options = { NULL, NULL, 320, 200, 30, 24, FALSE, FALSE, FALSE, TRUE };
static Options options = { NULL, NULL, -1, -1, -1, -1, FALSE, FALSE, FALSE, TRUE };
static ConfigWin *config_win = NULL;

static gboolean options_loaded = FALSE;
static gboolean fullscreen;
static gboolean gl_plugins_only;
static gboolean non_gl_plugins_only;
static gboolean all_plugins_enabled;
static int fps;

static void sync_options (void);

/* Callbacks */
static void on_button_ok_clicked (GtkButton *button, gpointer user_data);
static void on_button_apply_clicked (GtkButton *button, gpointer user_data);
static void on_button_cancel_clicked (GtkButton *button, gpointer user_data);

static void on_checkbutton_fullscreen_toggled (GtkToggleButton *togglebutton, gpointer user_data);

static void on_radiobutton_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data);
static void on_radiobutton_non_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data);
static void on_radiobutton_all_plugins_toggled (GtkToggleButton *togglebutton, gpointer user_data);

static void on_spinbutton_fps_changed (GtkEditable *editable, gpointer user_data);

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
	gchar *enabled_plugins;

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
			if (!xmms_cfg_read_string (f, "libvisual_xmms", "enabled_plugins", &enabled_plugins)) {
				options.gl_plugins_only = default_options.gl_plugins_only;
				options.non_gl_plugins_only = default_options.non_gl_plugins_only;
				options.all_plugins_enabled = default_options.all_plugins_enabled;
				errors = TRUE;
			} else {
				options.gl_plugins_only = FALSE;
				options.non_gl_plugins_only = FALSE;
				options.all_plugins_enabled = FALSE;
				if (strcmp (enabled_plugins, "gl_only") == 0)
					options.gl_plugins_only = TRUE;
				else if (strcmp (enabled_plugins, "non_gl_only") == 0)
					options.non_gl_plugins_only = TRUE;
				else if (strcmp (enabled_plugins, "all") == 0)
					options.all_plugins_enabled = TRUE;
				else {
					g_warning ("Invalid value for 'enabled_plugins' option");
					options.gl_plugins_only = default_options.gl_plugins_only;
					options.non_gl_plugins_only = default_options.non_gl_plugins_only;
					options.all_plugins_enabled = default_options.all_plugins_enabled;
					errors = TRUE;	
				}
				g_free (enabled_plugins);
			}
		} else {
			must_update = TRUE;
		}
		g_free (vstr);
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
		options.gl_plugins_only = default_options.gl_plugins_only;
		options.non_gl_plugins_only = default_options.non_gl_plugins_only;
		options.all_plugins_enabled = default_options.all_plugins_enabled;
	}

	xmms_cfg_free (f);

	/*
	 * Set our local copies
	 */
	fullscreen = options.fullscreen;
	fps = options.fps;
        gl_plugins_only = options.gl_plugins_only;
        non_gl_plugins_only = options.non_gl_plugins_only;
        all_plugins_enabled = options.all_plugins_enabled;
	if (gl_plugins_only)
		g_message ("GL plugins only");
	else if (non_gl_plugins_only)
		g_message ("non GL plugins only");
	else if (all_plugins_enabled)
		g_message ("All plugins enabled");
	else
		g_critical ("Cannot determine which kind of plugin to run");

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

	options_loaded = TRUE;

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
	if (options.gl_plugins_only)
		xmms_cfg_write_string (f, "libvisual_xmms", "enabled_plugins", "gl_only");
	else if (options.non_gl_plugins_only)
		xmms_cfg_write_string (f, "libvisual_xmms", "enabled_plugins", "non_gl_only");
	else if (options.all_plugins_enabled)
		xmms_cfg_write_string (f, "libvisual_xmms", "enabled_plugins", "all");
	else
		g_critical ("Inconsistency on config module");

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);

	return 0;
}

void lv_xmms_config_toggle_fullscreen (void)
{
	fullscreen = !fullscreen;
	options.fullscreen = !options.fullscreen;
	if (config_win != NULL)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen), fullscreen);
}

void lv_xmms_config_window ()
{
	setlocale (LC_MESSAGES, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	textdomain (GETTEXT_PACKAGE);
    
	if (config_win != NULL) {
		gtk_widget_show (config_win->window_main);
		return;
	}

	config_win = lv_xmms_config_gui_new ();

	if (options_loaded) {
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), options.fps);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen), options.fullscreen);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_opengl), options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_non_opengl), options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins), options.all_plugins_enabled);
        } else {
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), default_options.fps);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen), default_options.fullscreen);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_opengl), default_options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_non_opengl), default_options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins), default_options.all_plugins_enabled);
        }

	gtk_signal_connect (GTK_OBJECT (config_win->checkbutton_fullscreen), "toggled",
                      GTK_SIGNAL_FUNC (on_checkbutton_fullscreen_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->radiobutton_opengl), "toggled",
                      GTK_SIGNAL_FUNC (on_radiobutton_opengl_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->radiobutton_non_opengl), "toggled",
                      GTK_SIGNAL_FUNC (on_radiobutton_non_opengl_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->radiobutton_all_plugins), "toggled",
                      GTK_SIGNAL_FUNC (on_radiobutton_all_plugins_toggled),
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
	gtk_signal_connect (GTK_OBJECT (config_win->button_apply), "clicked",
                      GTK_SIGNAL_FUNC (on_button_apply_clicked),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->button_cancel), "clicked",
                      GTK_SIGNAL_FUNC (on_button_cancel_clicked),
                      NULL);

	gtk_widget_grab_default (config_win->button_cancel);

	gtk_widget_show (config_win->window_main);
}

static void on_checkbutton_fullscreen_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	fullscreen = !fullscreen;
}

static void on_radiobutton_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	gl_plugins_only = !gl_plugins_only;
}

static void on_radiobutton_non_opengl_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	non_gl_plugins_only = !non_gl_plugins_only;
}

static void on_radiobutton_all_plugins_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	all_plugins_enabled = !all_plugins_enabled;
}

static void on_spinbutton_fps_changed (GtkEditable *editable, gpointer user_data)
{
	gchar *buffer;

	buffer = gtk_editable_get_chars (editable, (gint) 0, (gint) -1);
	fps = atoi (buffer);
	g_free (buffer);
}


static gboolean on_pixmap_icon_button_press_event (GtkWidget *widget,
						GdkEventButton *event,
						gpointer user_data)
{

  return FALSE;
}


static void on_button_ok_clicked (GtkButton *button, gpointer user_data)
{
	sync_options ();
	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET(button)));
}

static void on_button_apply_clicked (GtkButton *button, gpointer user_data)
{
	sync_options ();
}


static void on_button_cancel_clicked (GtkButton *button, gpointer user_data)
{
	/*
	 * Restore original values
	 */
	if (options_loaded) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen),
						options.fullscreen);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), options.fps);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_opengl), options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_non_opengl), options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins), options.all_plugins_enabled);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen),
						default_options.fullscreen);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), default_options.fps);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_opengl), default_options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_non_opengl), default_options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins), default_options.all_plugins_enabled);
	}

	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET(button)));
}

static void sync_options ()
{
	options.fullscreen = fullscreen;
	options.fps = fps;
        options.gl_plugins_only = gl_plugins_only;
        options.non_gl_plugins_only = non_gl_plugins_only;
        options.all_plugins_enabled = all_plugins_enabled;
}

