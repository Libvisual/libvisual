#include <stdlib.h>

#include <libvisual/libvisual.h>

#include "avs_parse.h"

int main (int argc, char *argv[])
{
	AVSTree *avstree;
	char *file;

    visual_init(0, 0);

	if (argc < 2) {
		printf ("Usage: %s source\n", argv[0]);
	
		exit (-1);
	}

    VisParamContainer *pcont;

    pcont = visual_param_container_new();

    VisParamEntry *param = visual_param_entry_new(VIS_BSTR("TEST"));

    printf("param1: %p\n", param);

    visual_param_container_add(pcont, param);

    printf("fetch: %p\n", visual_hashmap_get_string(&pcont->entries, "TEST"));

    VisCollectionIterator iter;

    visual_collection_get_iterator( &iter, VISUAL_COLLECTION (&pcont->entries));

    while( visual_collection_iterator_has_more(&iter)) {
        VisHashmapChainEntry *outp = visual_collection_iterator_get_data(&iter);
        printf("param out %p\n", outp->data);

        //printf("name: %s\n", visual_string_get_cstring(visual_param_entry_get_name(outp)));
    }

	avstree = avs_tree_new_from_preset (argv[1]);


	visual_object_unref (VISUAL_OBJECT (avstree));

	return 0;
}

