#include <gtk/gtk.h>


typedef struct {
	GtkWidget *window_main;

	GtkWidget *optionmenu_vis_plugin;
	GSList *optionmenu_vis_plugin_group;
	GtkWidget *button_vis_plugin_conf;
	GtkWidget *button_vis_plugin_about;
	GtkWidget *checkbutton_fullscreen;
	GtkWidget *radiobutton_onlygl;
	GtkWidget *radiobutton_onlynongl;
	GtkWidget *radiobutton_all_plugins;
	GtkWidget *spinbutton_fps;

	GtkWidget *optionmenu_input_plugin;
	GSList *optionmenu_input_plugin_group;
	GtkWidget *button_input_plugin_conf;
	GtkWidget *button_input_plugin_about;
	
	GtkWidget *optionmenu_morph_plugin;
	GSList *optionmenu_morph_plugin_group;
	GtkWidget *button_morph_plugin_conf;
	GtkWidget *button_morph_plugin_about;
	
	GtkWidget *pixmap_icon;
	GtkWidget *button_ok;
	GtkWidget *button_apply;
	GtkWidget *button_cancel;
} ConfigWin;


ConfigWin *lv_xmms_config_gui_new (void);

