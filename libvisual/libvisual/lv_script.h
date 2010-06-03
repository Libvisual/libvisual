#ifndef _LV_SCRIPT_H
#define _LV_SCRIPT_H


#include <libvisual/lv_plugin.h>

#define VISUAL_PLUGIN_TYPE_SCRIPT	"Libvisual:core:script"

typedef struct _VisScript VisScript;
typedef struct _VisScriptPlugin VisScriptPlugin;

//typedef void * (*VisPluginScriptGetGlobal)(VisPluginData *plugin);
//typedef void * (*VisPluginScriptGetContext)(VisPluginData *plugin)

struct _VisScript {
	VisObject object;	/**< The VisObject data. */
	VisPluginData *plugin;	/**< Pointer to the plugin itself. */

};

struct _VisScriptPlugin {
	VisObject object;	/**<The VisObject data. */

	//VisPluginScriptGetGlobal *get_global;
	//VisPluginScriptGetContext *get_context;
};


#endif
