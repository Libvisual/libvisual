#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

namespace {

unsigned int const n_tile_rows = 4;
unsigned int const n_tile_cols = 4;

struct CheckerPrivate
{
    LV::Timer timer;
    bool flip;
};

int lv_morph_checkers_init (VisPluginData *plugin);
int lv_morph_checkers_cleanup (VisPluginData *plugin);
int lv_morph_checkers_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

} // namespace

const VisPluginInfo *get_plugin_info (void)
{
    static VisMorphPlugin morph;
    morph.apply = lv_morph_checkers_apply;
    morph.vidoptions.depth =
            VisVideoDepth (VISUAL_VIDEO_DEPTH_8BIT  |
                           VISUAL_VIDEO_DEPTH_16BIT |
                           VISUAL_VIDEO_DEPTH_24BIT |
                           VISUAL_VIDEO_DEPTH_32BIT);

    static VisPluginInfo info;
    info.type = VISUAL_PLUGIN_TYPE_MORPH;

    info.plugname = "checkers";
    info.name = "Checkerboard morph";
    info.author = "Scott Sibley <sisibley@gmail.com>";
    info.version = "0.1";
    info.about = N_("A checkers in/out morph plugin");
    info.help = N_("This morph plugin adds a checkerboard effect..");
    info.license = VISUAL_PLUGIN_LICENSE_LGPL;
    info.init = lv_morph_checkers_init;
    info.cleanup = lv_morph_checkers_cleanup;
    info.plugin = VISUAL_OBJECT (&morph);

    return &info;
}

namespace {
int lv_morph_checkers_init (VisPluginData *plugin)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    CheckerPrivate *priv = new CheckerPrivate;
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->timer.start ();

    return 0;
}

int lv_morph_checkers_cleanup (VisPluginData *plugin)
{
    CheckerPrivate *priv = (CheckerPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

    delete priv;

    return 0;
}

int lv_morph_checkers_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
    CheckerPrivate *priv = static_cast<CheckerPrivate*>(visual_object_get_private (VISUAL_OBJECT (plugin)));

    if(priv->timer.elapsed().to_msecs() > 300)
    {
        priv->flip = !priv->flip;
        priv->timer.reset ();
        priv->timer.start ();
    }

    dest->fill_color (LV::Color::black ());

    unsigned int dest_width  = visual_video_get_width (dest);
    unsigned int dest_height = visual_video_get_height (dest);

    unsigned int tile_width  = dest_width  / n_tile_cols;
    unsigned int tile_height = dest_height / n_tile_rows;

    LV::Rect subregion(0, 0, tile_width, tile_height);

    for(unsigned int row = 0, y = 0; y < dest_height; row++, y += tile_height)
    {
        for(unsigned int col = 0, x = 0; x < dest_width; col++, x += tile_width)
        {
            LV::VideoConstPtr src = (row + col + priv->flip) & 1 ? src1 : src2;
            LV::Rect region(x, y, tile_width, tile_height);

            LV::VideoPtr sub = LV::Video::create_sub(src, region);
            dest->blit(region, sub, subregion, false);
        }
    }

    return 0;
}
} // namespace

