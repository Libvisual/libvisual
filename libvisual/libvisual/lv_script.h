#ifndef _LV_SCRIPT_H
#define _LV_SCRIPT_H


#include <libvisual/lv_plugin.h>

#define VISUAL_PLUGIN_TYPE_SCRIPT	"Libvisual:core:script"

typedef struct _VisScript VisScript;
typedef struct _VisScriptPlugin VisScriptPlugin;

typedef void *(*VisPluginScriptGetContext)(VisPluginData *plugin);

struct _VisScript {
	VisObject object;	/**< The VisObject data. */
	VisPluginData *plugin;	/**< Pointer to the plugin itself. */

};

struct _VisScriptPlugin {
	VisObject object;	/**<The VisObject data. */

	VisPluginScriptGetContext *get_context; /**< Script context */
};

VisPluginData *visual_script_get_plugin (VisScript *script);

VisList *visual_script_get_list (void);

VisScript *visual_script_new (const char *scriptname);
int visual_script_init(VisScript *script, const char *scriptname);

int visual_script_realize (VisScript *morph);

#endif
