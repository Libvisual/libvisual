/*
 * lv-inspect can be used to get detailed information about a plugin.
 *
 * TODO:
 * -l option to give a summary list of all the plugins in the registry.
 * -d option to give a list of all plugins in detailed mode.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libvisual/libvisual.h>

char *get_plugin_typename_from_type (VisPluginType type);

void show_plugin_details (LVPlugin *plugin);

void show_plugin_info_actor (LVPlugin *plugin);
void show_plugin_info_input (LVPlugin *plugin);
void show_plugin_info_morph (LVPlugin *plugin);

void show_plugin_info (VisPluginInfo *info);
void show_plugin_ref (VisPluginRef *ref);

char *get_plugin_typename_from_type (VisPluginType type)
{
	switch (type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return "Null";
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			return "Actor";	
			break;
		
		case VISUAL_PLUGIN_TYPE_INPUT:
			return "Input";
			break;
		
		case VISUAL_PLUGIN_TYPE_MORPH:
			return "Morph";
			break;

		default:
			return "Invalid";
			break;
	}

	return "Invalid";
}

void show_plugin_details (LVPlugin *plugin)
{
	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_ACTOR:
			show_plugin_info_actor (plugin);
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			show_plugin_info_input (plugin);
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			show_plugin_info_morph (plugin);
			break;

		default:
			printf ("Invalid plugin type, can't show any detailed information about the plugin\n");
			break;
	}
}

void show_plugin_info (VisPluginInfo *info)
{
	printf ("Plugin information:\n");
	printf ("Plugin name: %s\n", info->name);
	printf ("Plugin author: %s\n", info->author);
	printf ("Plugin version: %s\n", info->version);
	printf ("Plugin about: %s\n", info->about);
	printf ("Plugin help: %s\n", info->help);
}

void show_plugin_ref (VisPluginRef *ref)
{
	printf ("Plugin shared object information:\n");
	printf ("plugin file: %s\n", ref->file);
}

void show_plugin_info_actor (LVPlugin *plugin)
{
	show_plugin_info (plugin->plugin.actorplugin->info);
	printf ("\n");
	show_plugin_ref (plugin->ref);
	printf ("\n");
	printf ("Actor plugin specific information:\n");
}

void show_plugin_info_input (LVPlugin *plugin)
{
	show_plugin_info (plugin->plugin.inputplugin->info);
	printf ("\n");
	show_plugin_ref (plugin->ref);
	printf ("\n");
	printf ("Input plugin specific information:\n");
}

void show_plugin_info_morph (LVPlugin *plugin)
{
	show_plugin_info (plugin->plugin.morphplugin->info);
	printf ("\n");
	show_plugin_ref (plugin->ref);
	printf ("\n");
	printf ("Morph plugin specific information:\n");
}

int main (int argc, char **argv)
{
	VisPluginRef *ref;
	LVPlugin *plugin;

	visual_init (&argc, &argv);

	if (argc < 2) {
		printf ("Usage: %s <pluginname>\n", argv[0]);
		return -1;
	}
	
	ref = visual_plugin_find (visual_plugin_get_registry (), argv[1]);

	if (ref == NULL) {
		printf ("Couldn't find plugin %s in the plugin registry\n", argv[1]);
		return -1;
	}

	plugin = visual_plugin_load (ref);
	visual_plugin_realize (plugin);	
	
	printf ("Plugin type: %s\n\n", get_plugin_typename_from_type (plugin->type));

	show_plugin_details (plugin);

	visual_plugin_unload (plugin);

	return 0;
}

