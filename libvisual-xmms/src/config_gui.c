#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config_gui.h"

static gchar *check_file_exists (const gchar *directory, const gchar *filename);
static GtkWidget *create_pixmap (GtkWidget *widget, const gchar *filename);
static GtkWidget *create_dummy_pixmap (GtkWidget *widget);
/*static GtkWidget* lookup_widget (GtkWidget *widget, const gchar *widget_name);*/
static void add_pixmap_directory (const gchar *directory);

ConfigWin *lv_xmms_config_gui_new (void)
{
  ConfigWin *config_gui;
  GtkWidget *window_main;
  GtkWidget *vbox_icon;
  GtkWidget *checkbutton_fullscreen;
  GtkWidget *checkbutton_opengl;
  GtkWidget *hbox_fps;
  GtkWidget *label_fps;
  GtkObject *spinbutton_fps_adj;
  GtkWidget *spinbutton_fps;
  GtkWidget *hbox_icon;
  GtkWidget *label_icon;
  GtkWidget *frame_icon;
  GtkWidget *pixmap_icon;
  GtkWidget *hseparator_bottom;
  GtkWidget *hbox_buttons;
  GtkWidget *button_ok;
  GtkWidget *button_cancel;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  window_main = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (window_main), "window_main", window_main);
  gtk_window_set_title (GTK_WINDOW (window_main), "LibVisual XMMS Plugin");
  gtk_window_set_position (GTK_WINDOW (window_main), GTK_WIN_POS_CENTER);
  gtk_window_set_policy (GTK_WINDOW (window_main), FALSE, FALSE, FALSE);

  vbox_icon = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox_icon);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "vbox_icon", vbox_icon,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox_icon);
  gtk_container_add (GTK_CONTAINER (window_main), vbox_icon);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_icon), 6);

  checkbutton_fullscreen = gtk_check_button_new_with_label ("Fullscreen");
  gtk_widget_ref (checkbutton_fullscreen);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "checkbutton_fullscreen", checkbutton_fullscreen,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton_fullscreen);
  gtk_box_pack_start (GTK_BOX (vbox_icon), checkbutton_fullscreen, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton_fullscreen,
		  "You can also toggle between normal and fullscreen mode by pressing key TAB or F11", NULL);

  checkbutton_opengl = gtk_check_button_new_with_label ("Enable OpenGl plugins");
  gtk_widget_ref (checkbutton_opengl);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "checkbutton_opengl", checkbutton_opengl,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton_opengl);
  gtk_box_pack_start (GTK_BOX (vbox_icon), checkbutton_opengl, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_opengl), TRUE);

  hbox_fps = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox_fps);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "hbox_fps", hbox_fps,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox_fps);
  gtk_box_pack_start (GTK_BOX (vbox_icon), hbox_fps, FALSE, FALSE, 0);

  label_fps = gtk_label_new ("Maximum Frames Per Second:");
  gtk_widget_ref (label_fps);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "label_fps", label_fps,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label_fps);
  gtk_box_pack_start (GTK_BOX (hbox_fps), label_fps, FALSE, FALSE, 6);
  gtk_label_set_justify (GTK_LABEL (label_fps), GTK_JUSTIFY_LEFT);

  spinbutton_fps_adj = gtk_adjustment_new (30, 10, 100, 1, 10, 10);
  spinbutton_fps = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_fps_adj), 1, 0);
  gtk_widget_ref (spinbutton_fps);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "spinbutton_fps", spinbutton_fps,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spinbutton_fps);
  gtk_box_pack_start (GTK_BOX (hbox_fps), spinbutton_fps, FALSE, FALSE, 0);

  hbox_icon = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox_icon);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "hbox_icon", hbox_icon,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox_icon);
  gtk_box_pack_start (GTK_BOX (vbox_icon), hbox_icon, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox_icon), 6);

  label_icon = gtk_label_new ("Icon");
  gtk_widget_ref (label_icon);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "label_icon", label_icon,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label_icon);
  gtk_box_pack_start (GTK_BOX (hbox_icon), label_icon, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label_icon), GTK_JUSTIFY_LEFT);

  frame_icon = gtk_frame_new (NULL);
  gtk_widget_ref (frame_icon);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "frame_icon", frame_icon,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame_icon);
  gtk_box_pack_start (GTK_BOX (hbox_icon), frame_icon, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame_icon), GTK_SHADOW_ETCHED_OUT);

  pixmap_icon = create_pixmap (window_main, "libvisual-xmms-vis.xpm");
  gtk_widget_ref (pixmap_icon);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "pixmap_icon", pixmap_icon,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap_icon);
  gtk_container_add (GTK_CONTAINER (frame_icon), pixmap_icon);

  hseparator_bottom = gtk_hseparator_new ();
  gtk_widget_ref (hseparator_bottom);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "hseparator_bottom", hseparator_bottom,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator_bottom);
  gtk_box_pack_start (GTK_BOX (vbox_icon), hseparator_bottom, TRUE, TRUE, 6);

  hbox_buttons = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox_buttons);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "hbox_buttons", hbox_buttons,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox_buttons);
  gtk_box_pack_start (GTK_BOX (vbox_icon), hbox_buttons, TRUE, TRUE, 0);

  button_ok = gtk_button_new_with_label ("Accept");
  gtk_widget_ref (button_ok);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "button_ok", button_ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_ok);
  gtk_box_pack_start (GTK_BOX (hbox_buttons), button_ok, FALSE, TRUE, 6);
  GTK_WIDGET_SET_FLAGS (button_ok, GTK_CAN_DEFAULT);

  button_cancel = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (button_cancel);
  gtk_object_set_data_full (GTK_OBJECT (window_main), "button_cancel", button_cancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_cancel);
  gtk_box_pack_start (GTK_BOX (hbox_buttons), button_cancel, FALSE, TRUE, 6);
  GTK_WIDGET_SET_FLAGS (button_cancel, GTK_CAN_DEFAULT);

  config_gui = g_new0 (ConfigWin, 1);

  config_gui->window_main = window_main;
  config_gui->checkbutton_fullscreen = checkbutton_fullscreen;
  config_gui->checkbutton_opengl = checkbutton_opengl;
  config_gui->spinbutton_fps = spinbutton_fps;
  config_gui->pixmap_icon = pixmap_icon;
  config_gui->button_ok = button_ok;
  config_gui->button_cancel = button_cancel;

  add_pixmap_directory (PACKAGE_DATADIR);
  
  return config_gui;
}

/* This is a dummy pixmap we use when a pixmap can't be found. */
static char *dummy_pixmap_xpm[] = {
/* columns rows colors chars-per-pixel */
"1 1 1 1",
"  c None",
/* pixels */
" "
};

static GtkWidget *create_dummy_pixmap (GtkWidget *widget)
{
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask,
                                                     NULL, dummy_pixmap_xpm);
  if (gdkpixmap == NULL)
    g_error ("Couldn't create replacement pixmap.");
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

static GList *pixmaps_directories = NULL;

static void add_pixmap_directory (const gchar *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}

static GtkWidget *create_pixmap (GtkWidget *widget, const gchar *filename)
{
  gchar *found_filename = NULL;
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;
  GList *elem;

  if (!filename || !filename[0])
      return create_dummy_pixmap (widget);

  /* We first try any pixmaps directories set by the application. */
  elem = pixmaps_directories;
  while (elem)
    {
      found_filename = check_file_exists ((gchar*)elem->data, filename);
      if (found_filename)
        break;
      elem = elem->next;
    }

  /* If we haven't found the pixmap, try the source directory. */
  if (!found_filename)
    {
      found_filename = check_file_exists ("../pixmaps", filename);
    }

  if (!found_filename)
    {
      g_warning ("Couldn't find pixmap file: %s", filename);
      return create_dummy_pixmap (widget);
    }

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
                                                   NULL, found_filename);
  if (gdkpixmap == NULL)
    {
      g_warning ("Error loading pixmap file: %s", found_filename);
      g_free (found_filename);
      return create_dummy_pixmap (widget);
    }
  g_free (found_filename);
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

static gchar *check_file_exists (const gchar *directory, const gchar *filename)
{
  gchar *full_filename;
  struct stat s;
  gint status;

  full_filename = (gchar*) g_malloc (strlen (directory) + 1
                                     + strlen (filename) + 1);
  strcpy (full_filename, directory);
  strcat (full_filename, G_DIR_SEPARATOR_S);
  strcat (full_filename, filename);

  status = stat (full_filename, &s);
  if (status == 0 && S_ISREG (s.st_mode))
    return full_filename;
  g_free (full_filename);
  return NULL;
}

/*static GtkWidget* lookup_widget (GtkWidget *widget, const gchar *widget_name)
{
  GtkWidget *parent, *found_widget;

  for (;;)
    {
      if (GTK_IS_MENU (widget))
        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
      else
        parent = widget->parent;
      if (parent == NULL)
        break;
      widget = parent;
    }

  found_widget = (GtkWidget*) gtk_object_get_data (GTK_OBJECT (widget),
                                                   widget_name);
  if (!found_widget)
    g_warning ("Widget not found: %s", widget_name);
  return found_widget;
}*/
