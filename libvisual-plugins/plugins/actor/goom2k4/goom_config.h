#if WORDS_BIGENDIAN
#define COLOR_ARGB
#else
#define COLOR_BGRA
#endif

#if 1
/* ndef COLOR_BGRA */
/** position des composantes **/
    #define BLEU 0
    #define VERT 1
    #define ROUGE 2
    #define ALPHA 3
#else
    #define ROUGE 1
    #define BLEU 3
    #define VERT 2
    #define ALPHA 0
#endif

#include <libvisual/lv_defines.h>

#ifndef M_PI
#include <libvisual/lv_math.h>
#define M_PI VISUAL_MATH_PI
#endif
