#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "lv_ui.h"

/**
 * @defgroup VisUI VisUI
 * @{
 */

VisUIWidget *visual_ui_widget_new (void)
{

}

int visual_ui_widget_free (VisUIWidget *widget)
{

}

int visual_ui_widget_destroy (VisUIWidget *widget)
{

}

VisUIWidget *visual_ui_widget_get_top (void)
{

}

VisUIWidget *visual_ui_widget_get_above (void)
{

}

VisUIWidget *visual_ui_widget_get_bellow (void)
{

}

VisUIWidgetType visual_ui_widget_get_type (VisUIWidget *widget)
{

}

VisUIWidget *visual_ui_container_new (void)
{

}

int visual_ui_container_add (VisUIContainer *container, VisUIWidget *widget)
{

}

VisUIWidget *visual_ui_container_get_child (VisUIContainer *container)
{

}

VisUIWidget *visual_ui_box_new (VisUIBoxType boxtype)
{

}

int visual_ui_box_pack (VisUIBox *box, VisUIWidget *widget)
{

}

VisUIWidget *visual_ui_box_get_next (VisUIBox *box, VisUIWidget *widget)
{

}

VisUIWidget *visual_ui_group_new (void)
{

}

VisUIWidget *visual_ui_label_new (const char *text)
{

}

int visual_ui_label_set_text (VisUILabel *label, const char *text)
{

}

const char *visual_ui_label_get_text (VisUILabel *label)
{

}

VisUIWidget *visual_ui_image_new (VisVideo *video)
{

}

int visual_ui_image_set_video (VisUIImage *image, const VisVideo *video)
{

}

const VisVideo *visual_ui_image_get_video (VisUIImage *image)
{

}

VisUIWidget *visual_ui_mutator_new (void)
{

}

int visual_ui_mutator_set_param (VisUIMutator *mutator, const VisParamEntry *param)
{

}

const VisParamEntry *visual_ui_mutator_get_param (VisUIMutator *mutator)
{

}

int visual_ui_mutator_set_max (VisUIMutator *mutator, double max)
{

}

int visual_ui_mutator_set_min (VisUIMutator *mutator, double min)
{

}

int visual_ui_mutator_set_step (VisUIMutator *mutator, double step)
{

}

VisUIWidget *visual_ui_text_new (void)
{

}

int visual_ui_text_set_length (VisUIText *text, int length)
{

}

VisUIWidget *visual_ui_slider_new (void)
{

}

VisUIWidget *visual_ui_numeric_new (void)
{

}

VisUIWidget *visual_ui_color_new (void)
{

}

VisUIWidget *visual_ui_choice_new (void)
{

}

/* FIXME helper functions, like list, entry create */

VisUIWidget *visual_ui_popup_new (void)
{

}

VisUIWidget *visual_ui_list_new (void)
{

}

VisUIWidget *visual_ui_radio_new (void)
{

}

/**
 * @}
 */

