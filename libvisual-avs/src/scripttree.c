#include "lvavs_script.h"

int main (int argc, char *argv[])
{
	LVAVSScriptTree *tree;
	LVAVSScriptTreeElement *elem, *elem2, *elem3;
	LVAVSScriptVMContext *vmctx;
	LVAVSScriptEnviron *senviron = lvavs_script_environ_new ();
	
	
	// SCRIPT: a = sin (b);
	// SCRIPT: c = cos (a * 2);


	tree = lvavs_script_tree_new ();

	/* Script 1 */
	elem = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION, "=");
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_VARIABLE, "a");
	lvavs_script_tree_element_add_leaf (elem, elem2);
	
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION, "sin");
	lvavs_script_tree_element_add_leaf (elem, elem2);

	elem3 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_VARIABLE, "b");
	lvavs_script_tree_element_add_leaf (elem2, elem3);
	
	lvavs_script_tree_add_element (tree, elem);

	
	/* Script 2 */
	elem = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION, "=");
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_VARIABLE, "c");
	lvavs_script_tree_element_add_leaf (elem, elem2);
	
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION, "cos");
	lvavs_script_tree_element_add_leaf (elem, elem2);

	elem3 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION, "*");
	lvavs_script_tree_element_add_leaf (elem2, elem3);
	
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_VARIABLE, "a");
	lvavs_script_tree_element_add_leaf (elem3, elem2);
	
	elem2 = lvavs_script_tree_element_new (LVAVS_SCRIPT_TREE_ELEMENT_TYPE_CONSTANT, "2");
	lvavs_script_tree_element_add_leaf (elem3, elem2);

	lvavs_script_tree_add_element (tree, elem);

	/* Zo dan */
	lvavs_script_vmcontext_set_environ (vmctx, senviron);
	vmctx = lvavs_script_vmcontext_new ();
	lvavs_script_vmcontext_compile (vmctx, tree);
	
	visual_object_unref (VISUAL_OBJECT (tree));

	return 0;
}

