#include <gtk/gtk.h>

#include "about.h"


/* About gui variables */
static GtkWidget *about_window = NULL;

static void gui_about_closed (GtkWidget *w, GdkEvent *e, gpointer data);
static void gui_about_destroy (GtkWidget *w, gpointer data);


void lv_xmms_about_show ()
{
	GtkWidget *vbox;
	GtkWidget *buttonbox;
	GtkWidget *close;
	GtkWidget *label;

	if (about_window != NULL)
		return;

	about_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (about_window), NULL);
	gtk_window_set_policy (GTK_WINDOW (about_window), FALSE, FALSE, FALSE);
	
	vbox = gtk_vbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (about_window), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
	gtk_widget_show (vbox);

	label = gtk_label_new("\n\
Libvisual xmms plugin\nCopyright (C) 2004, Dennis Smit <ds@nerds-incorporated.org>\n\
The libvisual xmms plugin, more information about libvisual can be found at\n\
http://libvisual.sf.net\n\n");

	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 8);
	gtk_widget_show (label);

	buttonbox = gtk_hbutton_box_new ();
	gtk_box_pack_end (GTK_BOX (vbox), buttonbox, FALSE, FALSE,8);
	gtk_widget_show (buttonbox);

	close = gtk_button_new_with_label ("Close");
	GTK_WIDGET_SET_FLAGS (close, GTK_CAN_DEFAULT);
	gtk_window_set_default (GTK_WINDOW (about_window), close);
	gtk_hbutton_box_set_layout_default (GTK_BUTTONBOX_END);
	gtk_box_pack_end (GTK_BOX (buttonbox), close, FALSE, FALSE,8);
	gtk_widget_show (close);

	gtk_signal_connect (GTK_OBJECT (close), "clicked", GTK_SIGNAL_FUNC (gui_about_closed), NULL);
	gtk_signal_connect (GTK_OBJECT (about_window), "delete-event", GTK_SIGNAL_FUNC (gui_about_destroy), NULL);
	      
	gtk_widget_show (about_window);
}


/*
 *
 * Private methods
 * 
 */

static void gui_about_closed (GtkWidget *w, GdkEvent *e, gpointer data)
{
	gui_about_destroy (about_window, data);
}

                                                                                                               
static void gui_about_destroy (GtkWidget *w, gpointer data)
{
	gtk_widget_destroy (w);
	about_window = NULL;
}

