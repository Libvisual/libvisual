#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <xmms/configfile.h>
#include <xmms/util.h>
#include <libvisual/libvisual.h>

#include "config.h"

#include "lv_xmms_config.h"
#include "config_gui.h"
#include "gettext.h"

#define CONFIG_DEFAULT_ICON (PACKAGE_DATADIR "/libvisual-xmms-vis.bmp")
#define CONFIG_DEFAULT_ACTOR_PLUGIN "infinite"
#define CONFIG_DEFAULT_INPUT_PLUGIN "esd"
#define CONFIG_DEFAULT_MORPH_PLUGIN "alphablend"

static const Options default_options = { NULL, NULL, NULL, NULL, 320, 200, 30, 24, FALSE, FALSE, FALSE, TRUE };
static Options options = { NULL, NULL, NULL, NULL, -1, -1, -1, -1, FALSE, FALSE, FALSE, TRUE };
static ConfigWin *config_win = NULL;

static gboolean options_loaded = FALSE;
static gboolean fullscreen;
static gboolean gl_plugins_only;
static gboolean non_gl_plugins_only;
static gboolean all_plugins_enabled;
static int fps;

static char *input_plugin = NULL;
static char *input_plugin_buffer = NULL;
static char *morph_plugin = NULL;
static char *morph_plugin_buffer = NULL;

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

static int config_win_load_input_plugin_list (void);
static void config_win_input_plugin_add (char *name);
static void on_input_plugin_activate (GtkMenuItem *menuitem, char *inputname);

static int config_win_load_morph_plugin_list (void);
static void config_win_morph_plugin_add (char *name);
static void on_morph_plugin_activate (GtkMenuItem *menuitem, char *morphname);
	
static void config_win_set_defaults (void);
static void config_win_connect_callbacks (void);
static void config_visual_initialize (void);

static gboolean read_config_file (ConfigFile *f);

static void dummy (GtkWidget *widget, gpointer data);

static void set_defaults (void);

static gboolean on_pixmap_icon_button_press_event (GtkWidget *widget,
						GdkEventButton *event,
						gpointer user_data);

Options *lv_xmms_config_open ()
{
	options.last_plugin = g_malloc0 (OPTIONS_MAX_NAME_LEN);
	input_plugin_buffer = g_malloc0 (OPTIONS_MAX_NAME_LEN);
	morph_plugin_buffer = g_malloc0 (OPTIONS_MAX_NAME_LEN);
	options.icon_file = g_malloc0 (OPTIONS_MAX_ICON_PATH_LEN);

	config_visual_initialize ();

	return &options;
}

int lv_xmms_config_close ()
{
	if (options.last_plugin != NULL)
		g_free (options.last_plugin);
	if (input_plugin_buffer != NULL)
		g_free (input_plugin_buffer);
	if (morph_plugin_buffer != NULL)
		g_free (morph_plugin_buffer);
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
	GtkWidget *msg;

	if ((f = xmms_cfg_open_default_file ()) == NULL)
		return -1;

	errors = FALSE;
	must_create_entry = FALSE;
	must_update = FALSE;
	if (xmms_cfg_read_string (f, "libvisual_xmms", "version", &vstr)) {
		if (strcmp (vstr, VERSION) == 0)
			errors = read_config_file (f);
		else
			must_update = TRUE;
		g_free (vstr);
	} else {
		must_create_entry = TRUE;
	}
	
	if (must_update || must_create_entry)
		set_defaults ();

	xmms_cfg_free (f);

	/*
	 * Set our local copies
	 */
	if (!visual_input_valid_by_name (input_plugin_buffer)) {
		msg = xmms_show_message (PACKAGE_NAME,
					_("The input plugin specified on the config\n"
					"file is not a valid input plugin.\n"
					"We will use "CONFIG_DEFAULT_INPUT_PLUGIN
					" input plugin instead.\n"
					"If you want another one, please choose it\n"
					"on the configure dialog."),
					_("Accept"), TRUE, dummy, NULL);
		gtk_widget_show (msg);
		strcpy (input_plugin_buffer, CONFIG_DEFAULT_INPUT_PLUGIN);
	}
	options.input_plugin = input_plugin_buffer;
	input_plugin = input_plugin_buffer;

	if (!visual_morph_valid_by_name (morph_plugin_buffer)) {
		msg = xmms_show_message (PACKAGE_NAME,
					_("The morph plugin specified on the config\n"
					"file is not a valid morph plugin.\n"
					"We will use "CONFIG_DEFAULT_MORPH_PLUGIN
					" morph plugin instead.\n"
					"If you want another one, please choose it\n"
					"on the configure dialog."),
					_("Accept"), TRUE, dummy, NULL);
		gtk_widget_show (msg);
		strcpy (morph_plugin_buffer, CONFIG_DEFAULT_MORPH_PLUGIN);
	}
	options.morph_plugin = morph_plugin_buffer;
	morph_plugin = morph_plugin_buffer;

	fullscreen = options.fullscreen;
	fps = options.fps;
        gl_plugins_only = options.gl_plugins_only;
        non_gl_plugins_only = options.non_gl_plugins_only;
        all_plugins_enabled = options.all_plugins_enabled;
	if (gl_plugins_only)
		g_message (_("GL plugins only"));
	else if (non_gl_plugins_only)
		g_message (_("non GL plugins only"));
	else if (all_plugins_enabled)
		g_message (_("All plugins enabled"));
	else
		g_critical ("Cannot determine which kind of plugin to run");

	if (errors) {
		g_message (_("LibVisual XMMS plugin: config file contain errors, fixing..."));
		lv_xmms_config_save_prefs ();
	} else if (must_update) {
		g_message (_("LibVisual XMMS plugin: config file is from old version, updating..."));
		lv_xmms_config_save_prefs ();
	} else if (must_create_entry) {
		g_message (_("LibVisual XMMS plugin: adding entry to config file..."));
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
		xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", CONFIG_DEFAULT_ACTOR_PLUGIN);

	if (options.input_plugin != NULL && (strlen(options.input_plugin) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "input_plugin", options.input_plugin);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "input_plugin", CONFIG_DEFAULT_INPUT_PLUGIN);
		
	if (options.morph_plugin != NULL && (strlen(options.morph_plugin) > 0))
		xmms_cfg_write_string (f, "libvisual_xmms", "morph_plugin", options.morph_plugin);
	else
		xmms_cfg_write_string (f, "libvisual_xmms", "morph_plugin", CONFIG_DEFAULT_MORPH_PLUGIN);
		
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
		g_warning ("Inconsistency on config module");

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);

	return 0;
}

void lv_xmms_config_toggle_fullscreen (void)
{
	fullscreen = !fullscreen;
	options.fullscreen = !options.fullscreen;
	if (config_win != NULL)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen),
							fullscreen);
}

void lv_xmms_config_window ()
{
#if ENABLE_NLS
	setlocale (LC_MESSAGES, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	textdomain (GETTEXT_PACKAGE);
#endif
    
	if (config_win != NULL) {
		gtk_widget_show (config_win->window_main);
		return;
	}

	config_visual_initialize ();

	config_win = lv_xmms_config_gui_new ();

	if (options_loaded) {
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), options.fps);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen),
							options.fullscreen);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlygl),
							options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlynongl),
							options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins),
							options.all_plugins_enabled);
        } else {
		config_win_set_defaults ();
        }

	config_win_connect_callbacks ();

	gtk_widget_grab_default (config_win->button_cancel);

	config_win_load_input_plugin_list ();
	config_win_load_morph_plugin_list ();
	
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
	lv_xmms_config_save_prefs ();
	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET(button)));
}

static void on_button_apply_clicked (GtkButton *button, gpointer user_data)
{
	sync_options ();
	lv_xmms_config_save_prefs ();
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
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlygl), options.gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlynongl), options.non_gl_plugins_only);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins), options.all_plugins_enabled);
	} else {
		config_win_set_defaults ();
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
	options.input_plugin = input_plugin;
}

static int config_win_load_input_plugin_list (void)
{
	VisList *list;
	VisListEntry *item;
	VisPluginRef *input;
	GtkWidget *msg;

	list = visual_input_get_list ();
	if (!list) {
		visual_log (VISUAL_LOG_WARNING, _("The list of input plugins is empty."));
		return -1;
	}
	
	item = NULL;
	/* FIXME update to visual_list_is_empty() when ready */
	if (!(input = (VisPluginRef*) visual_list_next (list, &item))) {
		msg = xmms_show_message (PACKAGE_NAME,
					_("There are no input plugins, so\n"
					"visualization plugins will work\n"
					"without rendering pcm data."),
					_("Accept"), TRUE, dummy, NULL);
		return -1;
	}
	item = NULL;
	while ((input = (VisPluginRef*) visual_list_next (list, &item))) {
		if (!(input->info)) {
			visual_log (VISUAL_LOG_WARNING, _("There is no info for this plugin"));
			continue;
		}
		config_win_input_plugin_add (input->info->plugname);
	}

	return 0;
}

static void config_win_input_plugin_add (char *name)
{
	GtkWidget *menu;
	GtkWidget *menuitem;
	GSList *group;
	
	group = config_win->optionmenu_input_plugin_group;
	menu = gtk_option_menu_get_menu (GTK_OPTION_MENU(config_win->optionmenu_input_plugin));
	menuitem = gtk_radio_menu_item_new_with_label (group, name);
	group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(menuitem));
	gtk_menu_append (GTK_MENU(menu), menuitem);
	config_win->optionmenu_input_plugin_group = group;

	gtk_signal_connect (GTK_OBJECT(menuitem), "activate",
				GTK_SIGNAL_FUNC(on_input_plugin_activate),
				(gpointer) name);

	gtk_widget_show (menuitem);

	if (!strcmp (name, CONFIG_DEFAULT_INPUT_PLUGIN)) {
		gtk_menu_item_activate (GTK_MENU_ITEM(menuitem));
		gtk_option_menu_set_history (GTK_OPTION_MENU(config_win->optionmenu_input_plugin), 1);
	}
}

static void on_input_plugin_activate (GtkMenuItem *menuitem, char *name)
{
	visual_log_return_if_fail (name != NULL);

	options.input_plugin = name;
}

static void on_button_input_plugin_about_clicked (GtkButton *button, gpointer data)
{
	VisList *list;
	VisListEntry *item;
	VisPluginRef *input;
	gchar *msg;
	GtkWidget *msgwin;
		
	list = visual_input_get_list ();
	if (!list) {
		visual_log (VISUAL_LOG_WARNING, _("The list of input plugins is empty."));
		return;
	}

	item = NULL;
	while ((input = (VisPluginRef*) visual_list_next (list, &item))) {
		if (!(input->info)) {
			visual_log (VISUAL_LOG_WARNING, _("There is no info for this plugin"));
			continue;
		}
		if (strcmp (input->info->plugname, options.input_plugin) == 0) {
			msg = g_strconcat (input->info->name, "\n",
					_("Version: "), input->info->version, "\n",
					input->info->about, "\n",
					_("Author: "), input->info->author, "\n\n",
					input->info->help, 0);
			msgwin = xmms_show_message (PACKAGE_NAME, msg,
					_("Accept"), TRUE, dummy, NULL);
			gtk_widget_show (msgwin);
			g_free (msg);
			break;
		}
	}
}

static int config_win_load_morph_plugin_list ()
{
	VisList *list;
	VisListEntry *item;
	VisPluginRef *morph;
	GtkWidget *msg;

	list = visual_morph_get_list ();
	if (!list) {
		visual_log (VISUAL_LOG_WARNING, _("The list of morph plugins is empty."));
		return -1;
	}
	
	item = NULL;
	/* FIXME update to visual_list_is_empty() when ready */
	if (!(morph = (VisPluginRef*) visual_list_next (list, &item))) {
		msg = xmms_show_message (PACKAGE_NAME,
					_("There are no morph plugins, so switching\n"
					"between visualization plugins will be do it\n"
					"without any morphing."),
					_("Accept"), TRUE, dummy, NULL);
		return -1;
	}
	item = NULL;
	while ((morph = (VisPluginRef*) visual_list_next (list, &item))) {
		if (!(morph->info)) {
			visual_log (VISUAL_LOG_WARNING, _("There is no info for this plugin"));
			continue;
		}
		config_win_morph_plugin_add (morph->info->plugname);
	}

	return 0;
}

static void config_win_morph_plugin_add (char *name)
{
	GtkWidget *menu;
	GtkWidget *menuitem;
	GSList *group;
	
	group = config_win->optionmenu_morph_plugin_group;
	menu = gtk_option_menu_get_menu (GTK_OPTION_MENU(config_win->optionmenu_morph_plugin));
	menuitem = gtk_radio_menu_item_new_with_label (group, name);
	group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(menuitem));
	gtk_menu_append (GTK_MENU(menu), menuitem);
	config_win->optionmenu_morph_plugin_group = group;

	gtk_signal_connect (GTK_OBJECT(menuitem), "activate",
				GTK_SIGNAL_FUNC(on_morph_plugin_activate),
				(gpointer) name);

	gtk_widget_show (menuitem);

	if (!strcmp (name, CONFIG_DEFAULT_MORPH_PLUGIN)) {
		gtk_menu_item_activate (GTK_MENU_ITEM(menuitem));
		gtk_option_menu_set_history (GTK_OPTION_MENU(config_win->optionmenu_morph_plugin), 1);
	}
}

static void on_morph_plugin_activate (GtkMenuItem *menuitem, char *name)
{
	visual_log_return_if_fail (name != NULL);

	options.morph_plugin = name;
}

static void on_button_morph_plugin_about_clicked (GtkButton *button, gpointer data)
{
	VisList *list;
	VisListEntry *item;
	VisPluginRef *morph;
	gchar *msg;
	GtkWidget *msgwin;
		
	list = visual_morph_get_list ();
	if (!list) {
		visual_log (VISUAL_LOG_WARNING, _("The list of input plugins is empty."));
		return;
	}

	item = NULL;
	while ((morph = (VisPluginRef*) visual_list_next (list, &item))) {
		if (!(morph->info)) {
			visual_log (VISUAL_LOG_WARNING, _("There is no info for this plugin"));
			continue;
		}
		if (strcmp (morph->info->plugname, options.morph_plugin) == 0) {
			msg = g_strconcat (morph->info->name, "\n",
					_("Version: "), morph->info->version, "\n",
					morph->info->about, "\n",
					_("Author: "), morph->info->author, "\n\n",
					morph->info->help, 0);
			msgwin = xmms_show_message (PACKAGE_NAME, msg,
					_("Accept"), TRUE, dummy, NULL);
			gtk_widget_show (msgwin);
			g_free (msg);
			break;
		}
	}
}

/*
 * This function set the default values for all options, except the selected
 * vis, input and morph plugins.
 */
static void config_win_set_defaults (void)
{
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->checkbutton_fullscreen),
					default_options.fullscreen);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(config_win->spinbutton_fps), default_options.fps);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlygl),
					default_options.gl_plugins_only);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_onlynongl),
					default_options.non_gl_plugins_only);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (config_win->radiobutton_all_plugins),
					default_options.all_plugins_enabled);
}

static void config_win_connect_callbacks (void)
{
	gtk_signal_connect (GTK_OBJECT (config_win->checkbutton_fullscreen), "toggled",
                      GTK_SIGNAL_FUNC (on_checkbutton_fullscreen_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->radiobutton_onlygl), "toggled",
                      GTK_SIGNAL_FUNC (on_radiobutton_opengl_toggled),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->radiobutton_onlynongl), "toggled",
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
	gtk_signal_connect (GTK_OBJECT (config_win->button_input_plugin_about), "clicked",
                      GTK_SIGNAL_FUNC (on_button_input_plugin_about_clicked),
                      NULL);
	gtk_signal_connect (GTK_OBJECT (config_win->button_morph_plugin_about), "clicked",
                      GTK_SIGNAL_FUNC (on_button_morph_plugin_about_clicked),
                      NULL);
}

static void set_defaults (void)
{
	strcpy (options.last_plugin, CONFIG_DEFAULT_ACTOR_PLUGIN);
	strcpy (input_plugin_buffer, CONFIG_DEFAULT_INPUT_PLUGIN);
	options.input_plugin = input_plugin_buffer;
	strcpy (morph_plugin_buffer, CONFIG_DEFAULT_MORPH_PLUGIN);
	options.morph_plugin = morph_plugin_buffer;

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

static void config_visual_initialize ()
{
	int argc;
	char **argv;
	GtkWidget *msg;

	if (!visual_is_initialized ()) {
	        argv = g_malloc (sizeof(char*));
	        argv[0] = g_strdup ("XMMS plugin");
        	argc = 1;
		if (visual_init (&argc, &argv) < 0) {
			msg = xmms_show_message (PACKAGE_NAME,
					_("We cannot initialize Libvisual library.\n"
					"Libvisual is necessary for this plugin to work."),
					_("Accept"), TRUE, dummy, NULL);
			gtk_widget_show (msg);
        		g_free (argv[0]);
		        g_free (argv);
			return;
		}
        	g_free (argv[0]);
	        g_free (argv);
	}
}

static void dummy (GtkWidget *widget, gpointer data)
{
}

static gboolean read_config_file (ConfigFile *f)
{
	gchar *enabled_plugins;
	gboolean errors = FALSE;

	if (!xmms_cfg_read_string (f, "libvisual_xmms", "last_plugin", &options.last_plugin)
 		|| (strlen (options.last_plugin) <= 0)) {
		strcpy (options.last_plugin, CONFIG_DEFAULT_ACTOR_PLUGIN);
		errors = TRUE;
	}
	if (!xmms_cfg_read_string (f, "libvisual_xmms", "input_plugin", &input_plugin_buffer)					|| (strlen (input_plugin_buffer) <= 0)) {
		strcpy (input_plugin_buffer, CONFIG_DEFAULT_INPUT_PLUGIN);
		errors = TRUE;
	}
	if (!xmms_cfg_read_string (f, "libvisual_xmms", "morph_plugin", &morph_plugin_buffer)					|| (strlen (morph_plugin_buffer) <= 0)) {
		strcpy (morph_plugin_buffer, CONFIG_DEFAULT_MORPH_PLUGIN);
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
			g_warning (_("Invalid value for 'enabled_plugins' option"));
			options.gl_plugins_only = default_options.gl_plugins_only;
			options.non_gl_plugins_only = default_options.non_gl_plugins_only;
			options.all_plugins_enabled = default_options.all_plugins_enabled;
			errors = TRUE;	
		}
	}
	g_free (enabled_plugins);

	return errors;
}
