#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "lv_log.h"
#include "lv_ui.h"

/**
 * @defgroup VisUI VisUI
 * @{
 */

VisUIWidget *visual_ui_widget_new ()
{
	VisUIWidget *widget;

	widget = visual_mem_new0 (VisUIWidget, 1);
	widget->type = VISUAL_WIDGET_TYPE_NULL;

	return widget;
}

int visual_ui_widget_free (VisUIWidget *widget)
{
	visual_log_return_val_if_fail (widget != NULL, -1);

	visual_mem_free (widget);

	return 0;
}

int visual_ui_widget_destroy (VisUIWidget *widget)
{
	visual_log_return_val_if_fail (widget != NULL, -1);

	/* FIXME create impl */

	return 0;
}

VisUIWidget *visual_ui_widget_get_top (VisUIWidget *widget)
{
	VisUIWidget *above;
	VisUIWidget *prev = widget;

	visual_log_return_val_if_fail (widget != NULL, NULL);

	while ((above = visual_ui_widget_get_parent (widget)) != NULL) {
		prev = widget;
	}

	return prev;
}

VisUIWidget *visual_ui_widget_get_parent (VisUIWidget *widget)
{
	visual_log_return_val_if_fail (widget != NULL, NULL);

	return widget->parent;
}

VisUIWidgetType visual_ui_widget_get_type (VisUIWidget *widget)
{
	visual_log_return_val_if_fail (widget != NULL, VISUAL_WIDGET_TYPE_NULL);

	return widget->type;
}

VisUIWidget *visual_ui_container_new ()
{
	VisUIContainer *container;

	container = visual_mem_new0 (VisUIContainer, 1);
	VISUAL_UI_WIDGET (container)->type = VISUAL_WIDGET_TYPE_CONTAINER;

	return VISUAL_UI_WIDGET (container);
}

int visual_ui_container_add (VisUIContainer *container, VisUIWidget *widget)
{
	visual_log_return_val_if_fail (container != NULL, -1);
	visual_log_return_val_if_fail (widget != NULL, -1);

	container->child = widget;

	return 0;
}

VisUIWidget *visual_ui_container_get_child (VisUIContainer *container)
{
	visual_log_return_val_if_fail (container != NULL, NULL);

	return container->child;
}

VisUIWidget *visual_ui_box_new (VisUIBoxType boxtype)
{
	VisUIBox *box;

	box = visual_mem_new0 (VisUIBox, 1);
	VISUAL_UI_WIDGET (box)->type = VISUAL_WIDGET_TYPE_BOX;

	box->boxtype = boxtype;

	return VISUAL_UI_WIDGET (box);
}

int visual_ui_box_pack (VisUIBox *box, VisUIWidget *widget)
{
	visual_log_return_val_if_fail (box != NULL, -1);
	visual_log_return_val_if_fail (widget != NULL, -1);

	visual_list_add (&box->childs, widget);

	return 0;
}

VisUIWidget *visual_ui_box_get_next (VisUIBox *box, VisUIWidget *widget)
{
	VisUIWidget *next;
	VisListEntry *le = NULL;

	visual_log_return_val_if_fail (box != NULL, NULL);

	while ((next = visual_list_next (&box->childs, &le)) != NULL) {

		if (widget == NULL)
			return next;

		/* Found current widget, let's return the next one */
		if (next == widget) {
			next = visual_list_next (&box->childs, &le);

			return next;
		}
	}
	
	return NULL;
}

VisUIWidget *visual_ui_group_new ()
{
	VisUIGroup *group;

	group = visual_mem_new0 (VisUIGroup, 1);
	VISUAL_UI_WIDGET (group)->type = VISUAL_WIDGET_TYPE_GROUP;

	return VISUAL_UI_WIDGET (group);
}

VisUIWidget *visual_ui_label_new (const char *text)
{
	VisUILabel *label;

	label = visual_mem_new0 (VisUILabel, 1);
	VISUAL_UI_WIDGET (label)->type = VISUAL_WIDGET_TYPE_LABEL;

	label->text = text;

	return VISUAL_UI_WIDGET (label);
}

int visual_ui_label_set_text (VisUILabel *label, const char *text)
{
	visual_log_return_val_if_fail (label != NULL, -1);

	label->text = text;

	return 0;
}

const char *visual_ui_label_get_text (VisUILabel *label)
{
	visual_log_return_val_if_fail (label != NULL, NULL);

	return label->text;
}

VisUIWidget *visual_ui_image_new (const VisVideo *video)
{
	VisUIImage *image;

	image = visual_mem_new0 (VisUIImage, 1);
	VISUAL_UI_WIDGET (image)->type = VISUAL_WIDGET_TYPE_IMAGE;

	image->image = video;

	return VISUAL_UI_WIDGET (image);
}

int visual_ui_image_set_video (VisUIImage *image, const VisVideo *video)
{
	visual_log_return_val_if_fail (image != NULL, -1);

	image->image = video;

	return 0;
}

const VisVideo *visual_ui_image_get_video (VisUIImage *image)
{
	visual_log_return_val_if_fail (image != NULL, NULL);

	return image->image;
}

VisUIWidget *visual_ui_mutator_new ()
{
	VisUIMutator *mutator;

	mutator = visual_mem_new0 (VisUIMutator, 1);
	VISUAL_UI_WIDGET (mutator)->type = VISUAL_WIDGET_TYPE_MUTATOR;

	return VISUAL_UI_WIDGET (mutator);
}

int visual_ui_mutator_set_param (VisUIMutator *mutator, const VisParamEntry *param)
{
	visual_log_return_val_if_fail (mutator != NULL, -1);
	visual_log_return_val_if_fail (param != NULL, -1);

	mutator->param = param;

	return 0;
}

const VisParamEntry *visual_ui_mutator_get_param (VisUIMutator *mutator)
{
	visual_log_return_val_if_fail (mutator != NULL, NULL);

	return mutator->param;
}

int visual_ui_mutator_set_max (VisUIMutator *mutator, double max)
{
	visual_log_return_val_if_fail (mutator != NULL, -1);

	mutator->max = max;

	return 0;
}

int visual_ui_mutator_set_min (VisUIMutator *mutator, double min)
{
	visual_log_return_val_if_fail (mutator != NULL, -1);

	mutator->min = min;

	return 0;
}

int visual_ui_mutator_set_step (VisUIMutator *mutator, double step)
{
	visual_log_return_val_if_fail (mutator != NULL, -1);

	mutator->step = step;

	return 0;
}

VisUIWidget *visual_ui_text_new ()
{
	VisUIText *text;

	text = visual_mem_new0 (VisUIText, 1);
	VISUAL_UI_WIDGET (text)->type = VISUAL_WIDGET_TYPE_TEXT;

	return VISUAL_UI_WIDGET (text);
}

int visual_ui_text_set_length (VisUIText *text, int length)
{
	visual_log_return_val_if_fail (text != NULL, -1);

	text->length = length;

	return 0;
}

VisUIWidget *visual_ui_slider_new ()
{
	VisUISlider *slider;

	slider = visual_mem_new0 (VisUISlider, 1);
	VISUAL_UI_WIDGET (slider)->type = VISUAL_WIDGET_TYPE_SLIDER;

	return VISUAL_UI_WIDGET (slider);
}

VisUIWidget *visual_ui_numeric_new ()
{
	VisUINumeric *numeric;

	numeric = visual_mem_new0 (VisUINumeric, 1);
	VISUAL_UI_WIDGET (numeric)->type = VISUAL_WIDGET_TYPE_NUMERIC;

	return VISUAL_UI_WIDGET (numeric);
}

VisUIWidget *visual_ui_color_new ()
{
	VisUIColor *color;

	color = visual_mem_new0 (VisUIColor, 1);
	VISUAL_UI_WIDGET (color)->type = VISUAL_WIDGET_TYPE_COLOR;

	return VISUAL_UI_WIDGET (color);
}

VisUIWidget *visual_ui_choice_new ()
{
	VisUIChoice *choice;

	choice = visual_mem_new0 (VisUIChoice, 1);
	VISUAL_UI_WIDGET (choice)->type = VISUAL_WIDGET_TYPE_CHOICE;

	return VISUAL_UI_WIDGET (choice);
}

/* FIXME helper functions, like list, entry create */

VisUIWidget *visual_ui_popup_new ()
{
	VisUIPopup *popup;

	popup = visual_mem_new0 (VisUIPopup, 1);
	VISUAL_UI_WIDGET (popup)->type = VISUAL_WIDGET_TYPE_POPUP;

	return VISUAL_UI_WIDGET (popup);
}

VisUIWidget *visual_ui_list_new ()
{
	VisUIList *list;

	list = visual_mem_new0 (VisUIList, 1);
	VISUAL_UI_WIDGET (list)->type = VISUAL_WIDGET_TYPE_LIST;

	return VISUAL_UI_WIDGET (list);
}

VisUIWidget *visual_ui_radio_new ()
{
	VisUIRadio *radio;

	radio = visual_mem_new0 (VisUIRadio, 1);
	VISUAL_UI_WIDGET (radio)->type = VISUAL_WIDGET_TYPE_RADIO;
	
	return VISUAL_UI_WIDGET (radio);
}

VisUIWidget *visual_ui_checkbox_new ()
{
	VisUICheckbox *checkbox;

	checkbox = visual_mem_new0 (VisUICheckbox, 1);
	VISUAL_UI_WIDGET (checkbox)->type = VISUAL_WIDGET_TYPE_CHECKBOX;

	return VISUAL_UI_WIDGET (checkbox);
}

/**
 * @}
 */

