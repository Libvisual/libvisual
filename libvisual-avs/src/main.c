#include "avs_parse.h"

int main (int argc, char *argv[])
{
	AVSTree *avstree;
	char *file;

	if (argc < 2) {
		printf ("Usage: %s source\n", argv[0]);
	
		exit (-1);
	}

	avstree = avs_tree_new_from_preset (argv[1]);


	visual_object_unref (VISUAL_OBJECT (avstree));

	return 0;
}

