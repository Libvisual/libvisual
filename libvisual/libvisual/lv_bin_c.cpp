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
    visual_return_if_fail (bin != NULL);

    bin->realize ();
}

void visual_bin_set_actor (VisBin *bin, VisActor *actor)
{
    visual_return_if_fail (bin != NULL);

    bin->set_actor (actor);
}

VisActor *visual_bin_get_actor (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, NULL);

    return bin->get_actor ();
}

void visual_bin_set_input (VisBin *bin, VisInput *input)
{
    visual_return_if_fail (bin != NULL);

    bin->set_input (input);
}

VisInput *visual_bin_get_input (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, NULL);

    return bin->get_input ();
}

void visual_bin_set_morph (VisBin *bin, const char *morphname)
{
    visual_return_if_fail (bin != NULL);

    bin->set_morph (morphname);
}

VisMorph *visual_bin_get_morph (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, NULL);

    return bin->get_morph ();
}

void visual_bin_connect (VisBin *bin, VisActor *actor, VisInput *input)
{
    visual_return_if_fail (bin != NULL);

    bin->connect (actor, input);
}

void visual_bin_connect_by_names (VisBin *bin, const char *actname, const char *inname)
{
    visual_return_if_fail (bin != NULL);

    bin->connect (actname, inname);
}

void visual_bin_sync (VisBin *bin, int noevent)
{
    visual_return_if_fail (bin != NULL);

    bin->sync (noevent);
}

void visual_bin_set_video (VisBin *bin, VisVideo *video)
{
    visual_return_if_fail (bin != NULL);

    bin->set_video (LV::VideoPtr (video));
}

void visual_bin_set_supported_depth (VisBin *bin, VisVideoDepth depthflag)
{
    visual_return_if_fail (bin != NULL);

    bin->set_supported_depth (depthflag);
}

void visual_bin_set_preferred_depth (VisBin *bin, VisBinDepth depth)
{
    visual_return_if_fail (bin != NULL);

    bin->set_preferred_depth (depth);
}

void visual_bin_set_depth (VisBin *bin, VisVideoDepth depth)
{
    visual_return_if_fail (bin != NULL);

    bin->set_depth (depth);
}

VisVideoDepth visual_bin_get_depth (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, VISUAL_VIDEO_DEPTH_NONE);

    return bin->get_depth ();
}

int visual_bin_depth_changed (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, FALSE);

    return bin->depth_changed ();
}

const VisPalette *visual_bin_get_palette (VisBin *bin)
{
    visual_return_val_if_fail (bin != NULL, NULL);

    return &bin->get_palette ();
}

void visual_bin_switch_actor_by_name (VisBin *bin, const char *actname)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_actor (actname);
}

void visual_bin_switch_actor (VisBin *bin, VisActor *actor)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_actor (actor);
}

void visual_bin_switch_finalize (VisBin *bin)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_finalize ();
}

void visual_bin_switch_set_style (VisBin *bin, VisBinSwitchStyle style)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_style (style);
}

void visual_bin_switch_set_steps (VisBin *bin, int steps)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_steps (steps);
}

void visual_bin_switch_set_automatic (VisBin *bin, int automatic)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_automatic (automatic);
}

void visual_bin_switch_set_rate (VisBin *bin, float rate)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_rate (rate);
}

void visual_bin_switch_set_mode (VisBin *bin, VisMorphMode mode)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_mode (mode);
}

void visual_bin_switch_set_time (VisBin *bin, long sec, long usec)
{
    visual_return_if_fail (bin != NULL);

    bin->switch_set_time (LV::Time (sec, usec * VISUAL_NSEC_PER_USEC));
}

void visual_bin_run (VisBin *bin)
{
    visual_return_if_fail (bin != NULL);

    bin->run ();
}
