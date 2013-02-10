#include "goom_tools.h"
#include <libvisual/libvisual.h>
#include <stdlib.h>

GoomRandom *goom_random_init(int i) {
	GoomRandom *grandom = (GoomRandom*)malloc(sizeof(GoomRandom));
	grandom->pos = 1;
	goom_random_update_array(grandom, GOOM_NB_RAND);
	return grandom;
}

void goom_random_free(GoomRandom *grandom) {
	free(grandom);
}

void goom_random_update_array(GoomRandom *grandom, int numberOfValuesToChange) {
	while (numberOfValuesToChange > 0) {
		grandom->array[grandom->pos++] = visual_rand() / 127;
		numberOfValuesToChange--;
	}
}
