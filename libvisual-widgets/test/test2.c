#include <stdio.h>
#include <gtk/gtk.h>
#include <lvwidget/gtk2/lv_widget_gtk2_visui.h>


int main (int argc, char *argv[])
{
	VisUIWidget *lvwidget;
	VisActor *actor;
	GtkWidget *window, *widget;

	if (argc < 2) {
		fprintf (stderr, "Usage: %s actor\n", argv[0]);
		return 1;
	}

	gtk_init (&argc, &argv);
	visual_init (NULL, NULL);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
	actor = visual_actor_new (argv[1]);
	visual_actor_realize (actor);
	
	lvwidget = visual_plugin_get_userinterface (visual_actor_get_plugin (actor));
	widget = lvwidget_visui_new (lvwidget);

	gtk_container_add (GTK_CONTAINER(window), widget);

	gtk_widget_show_all (window);

	gtk_main ();
	
	return 0;
}

