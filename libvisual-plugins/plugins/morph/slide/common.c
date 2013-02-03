#include "common.h"
#include <libvisual/libvisual.h>

VisMorphPlugin morph = {
	.apply = lv_morph_slide_apply,
	.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	                  | VISUAL_VIDEO_DEPTH_16BIT
	                  | VISUAL_VIDEO_DEPTH_24BIT
	                  | VISUAL_VIDEO_DEPTH_32BIT
};

int lv_morph_slide_init (VisPluginData *plugin, SlideType type)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    SlidePrivate *priv = visual_mem_new0 (SlidePrivate, 1);
    visual_plugin_set_private (plugin, priv);

    priv->slide_type = type;

    return TRUE;
}

void lv_morph_slide_cleanup (VisPluginData *plugin)
{
    SlidePrivate *priv = visual_plugin_get_private (plugin);

    visual_mem_free (priv);
}

void lv_morph_slide_apply (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
    SlidePrivate *priv = visual_plugin_get_private (plugin);
    uint8_t *destbuf = visual_video_get_pixels (dest);
    uint8_t *srcbuf1 = visual_video_get_pixels (src1);
    uint8_t *srcbuf2 = visual_video_get_pixels (src2);

    int dest_height = visual_video_get_height (dest);
    int dest_pitch  = visual_video_get_pitch  (dest);

    int i;
    int diff1;
    int diff2;
    int hadd;

    visual_mem_set (destbuf, 0, visual_video_get_size (dest));

    if (priv->slide_type == SLIDE_RIGHT || priv->slide_type == SLIDE_UP)
        progress = 1.0 - progress;

    diff1 = dest_pitch * progress;
    diff1 -= diff1 % visual_video_get_bpp (dest);

    if (diff1 > dest_pitch)
        diff1 = dest_pitch;

    diff2 = dest_pitch - diff1;

    hadd = dest_height * progress;

    switch (priv->slide_type) {
        case SLIDE_LEFT:
            for (i = 0; i < dest_height; i++) {
                visual_mem_copy (destbuf + (i * dest_pitch), srcbuf2 + (i * dest_pitch) + diff2, diff1);
                visual_mem_copy (destbuf + (i * dest_pitch) + (diff1), srcbuf1 + (i * dest_pitch), diff2);
            }

            break;

        case SLIDE_RIGHT:
            for (i = 0; i < dest_height; i++) {
                visual_mem_copy (destbuf + (i * dest_pitch), srcbuf1 + (i * dest_pitch) + diff2, diff1);
                visual_mem_copy (destbuf + (i * dest_pitch) + (diff1), srcbuf2 + (i * dest_pitch), diff2);
            }

            break;

        case SLIDE_DOWN:
            visual_mem_copy (destbuf, srcbuf1 + (hadd * dest_pitch), (dest_height - hadd) * dest_pitch);
            visual_mem_copy (destbuf + ((dest_height - hadd) * dest_pitch), srcbuf2, hadd * dest_pitch);

            break;

        case SLIDE_UP:
            visual_mem_copy (destbuf, srcbuf2 + (hadd * dest_pitch), (dest_height - hadd) * dest_pitch);
            visual_mem_copy (destbuf + ((dest_height - hadd) * dest_pitch), srcbuf1, hadd * dest_pitch);

            break;

        default:
            visual_log (VISUAL_LOG_WARNING,
                    "Slide plugin is initialized with an impossible slide direction, this should never happen");

            break;
    }
}
