#include "common.h"
#include <libvisual/libvisual.h>

int lv_morph_slide_init (VisPluginData *plugin, SlideType type)
{
    SlidePrivate *priv = visual_mem_new0 (SlidePrivate, 1);
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->slide_type = type;

    return 0;
}

int lv_morph_slide_cleanup (VisPluginData *plugin)
{
    SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_mem_free (priv);

    return 0;
}

int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
    SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *destbuf = visual_video_get_pixels (dest);
    uint8_t *srcbuf1 = visual_video_get_pixels (src1);
    uint8_t *srcbuf2 = visual_video_get_pixels (src2);
    int i;
    int diff1;
    int diff2;
    int hadd;

    visual_mem_set (destbuf, 0, visual_video_get_size (dest));

    if (priv->slide_type == SLIDE_RIGHT || priv->slide_type == SLIDE_UP)
        rate = 1.0 - rate;

    diff1 = dest->pitch * rate;
    diff1 -= diff1 % dest->bpp;

    if (diff1 > dest->pitch)
        diff1 = dest->pitch;

    diff2 = dest->pitch - diff1;

    hadd = dest->height * rate;

    switch (priv->slide_type) {
        case SLIDE_LEFT:
            for (i = 0; i < dest->height; i++) {
                visual_mem_copy (destbuf + (i * dest->pitch), srcbuf2 + (i * dest->pitch) + diff2, diff1);
                visual_mem_copy (destbuf + (i * dest->pitch) + (diff1), srcbuf1 + (i * dest->pitch), diff2);
            }

            break;

        case SLIDE_RIGHT:
            for (i = 0; i < dest->height; i++) {
                visual_mem_copy (destbuf + (i * dest->pitch), srcbuf1 + (i * dest->pitch) + diff2, diff1);
                visual_mem_copy (destbuf + (i * dest->pitch) + (diff1), srcbuf2 + (i * dest->pitch), diff2);
            }

            break;

        case SLIDE_DOWN:
            visual_mem_copy (destbuf, srcbuf1 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
            visual_mem_copy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf2, hadd * dest->pitch);

            break;

        case SLIDE_UP:
            visual_mem_copy (destbuf, srcbuf2 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
            visual_mem_copy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf1, hadd * dest->pitch);

            break;

        default:
            visual_log (VISUAL_LOG_WARNING,
                    "Slide plugin is initialized with an impossible slide direction, this should never happen");

            break;
    }

    return 0;
}
