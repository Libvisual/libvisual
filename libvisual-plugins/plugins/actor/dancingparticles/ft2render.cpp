#include <stdio.h>
#include <stdlib.h>

#include "font.h"
#include "etoile.h"

#include <libvisual/libvisual.h>

int main(int argc, char ** argv) {
    printf("ptsNum %d\n", ptsNum);
    printf("ptsNumMax %d\n", ptsNumMax);
    printf("Centers %p\n\n", Centers);

    // This mimics allocParts() of parameters.cpp
    Centers = (FloatPoint *) malloc(ptsNumMax * sizeof(FloatPoint));

    // From actor_dancingparticles.cpp, apparently a restaurant in Amsterdam?
    LV::System::init(argc, argv);
    initFontRasterizer();
    loadString("Moeders");
    loadString("[1_345678][2_345678][3_345678][4_345678]THIS IS CUT OFF");
    destroyFontRasterizer();
    // LV::System::destroy();  // crashes with free(): invalid pointer

    return 0;
}
