#include "config.h"
#include "lv_bin.h"
#include "lv_common.h"

VisBin *visual_bin_new (void)
{
    return new LV::Bin;
}

void visual_bin_free (VisBin *bin)
{
    delete bin;
}

void visual_bin_realize (VisBin *bin)
{
    visual_return_if_fail (bin != nullptr);

    bin->realize ();
}

VisActor *visual_bin_get_actor (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, nullptr);

    return bin->get_actor ().get ();
}

VisInput *visual_bin_get_input (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, nullptr);

    return bin->get_input ().get ();
}

void visual_bin_set_morph (VisBin *bin, const char *morphname)
{
    visual_return_if_fail (bin != nullptr);

    bin->set_morph (morphname);
}

VisMorph *visual_bin_get_morph (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, nullptr);

    return bin->get_morph ().get ();
}

void visual_bin_connect (VisBin *bin, const char *actname, const char *inname)
{
    visual_return_if_fail (bin != nullptr);

    bin->connect (actname, inname);
}

void visual_bin_sync (VisBin *bin, int noevent)
{
    visual_return_if_fail (bin != nullptr);

    bin->sync (noevent);
}

void visual_bin_set_video (VisBin *bin, VisVideo *video)
{
    visual_return_if_fail (bin != nullptr);

    bin->set_video (LV::VideoPtr (video));
}

void visual_bin_set_supported_depth (VisBin *bin, VisVideoDepth depthflag)
{
    visual_return_if_fail (bin != nullptr);

    bin->set_supported_depth (depthflag);
}

void visual_bin_set_preferred_depth (VisBin *bin, VisBinDepth depth)
{
    visual_return_if_fail (bin != nullptr);

    bin->set_preferred_depth (depth);
}

void visual_bin_set_depth (VisBin *bin, VisVideoDepth depth)
{
    visual_return_if_fail (bin != nullptr);

    bin->set_depth (depth);
}

VisVideoDepth visual_bin_get_depth (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    return bin->get_depth ();
}

int visual_bin_depth_changed (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, FALSE);

    return bin->depth_changed ();
}

const VisPalette *visual_bin_get_palette (VisBin *bin)
{
    visual_return_val_if_fail (bin != nullptr, nullptr);

    return &bin->get_palette ();
}

void visual_bin_switch_actor (VisBin *bin, const char *actname)
{
    visual_return_if_fail (bin != nullptr);

    bin->switch_actor (actname);
}

void visual_bin_switch_finalize (VisBin *bin)
{
    visual_return_if_fail (bin != nullptr);

    bin->switch_finalize ();
}

void visual_bin_use_morph (VisBin *bin, int use)
{
    visual_return_if_fail (bin != nullptr);

    bin->use_morph (use);
}

void visual_bin_switch_set_time (VisBin *bin, long sec, long usec)
{
    visual_return_if_fail (bin != nullptr);

    bin->switch_set_time (LV::Time (sec, usec * VISUAL_NSECS_PER_USEC));
}

void visual_bin_run (VisBin *bin)
{
    visual_return_if_fail (bin != nullptr);

    bin->run ();
}
