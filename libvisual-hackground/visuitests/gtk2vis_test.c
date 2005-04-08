#include <lvwidget/gtk2/lv_widget_gtk2_visui.h>

#include <stdlib.h>

int main(int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *vuic;
	VisActor *actor;	

	VisUIWidget *main;

	gtk_init (&argc, &argv);
	visual_init (&argc,&argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

	gtk_window_set_title (GTK_WINDOW (window), "LV VisUI traverser test!");

	g_signal_connect (G_OBJECT (window), "destroy",
			G_CALLBACK (exit), NULL);

	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	if (argc < 2)
		actor = visual_actor_new ("bumpscope");
	else
		actor = visual_actor_new (argv[1]);

	if (actor == NULL) {
		printf ("jaja actor= nullox\n");

		return -1;
	}

	visual_actor_realize (actor);
	main = visual_plugin_get_userinterface (visual_actor_get_plugin (actor));
//	main = visual_get_userinterface ();

	
	vuic = lvwidget_visui_new (main);

	gtk_container_add (GTK_CONTAINER (window), vuic);
	gtk_widget_show_all (vuic);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}

