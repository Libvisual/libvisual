#include <gtk/gtk.h>


typedef struct {
	GtkWidget *window_main;
	GtkWidget *checkbutton_fullscreen;
	GtkWidget *radiobutton_opengl;
	GtkWidget *radiobutton_non_opengl;
	GtkWidget *radiobutton_all_plugins;
	GtkWidget *spinbutton_fps;
	GtkWidget *pixmap_icon;
	GtkWidget *button_ok;
	GtkWidget *button_apply;
	GtkWidget *button_cancel;
} ConfigWin;


ConfigWin *lv_xmms_config_gui_new (void);

