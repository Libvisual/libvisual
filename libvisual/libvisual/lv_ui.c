#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "lv_log.h"
#include "lv_ui.h"

/**
 * @defframe VisUI VisUI
 * @{
 */

VisUIWidget *visual_ui_widget_new ()
{
	VisUIWidget *widget;

	widget = visual_mem_new0 (VisUIWidget, 1);
	widget->type = VISUAL_WIDGET_TYPE_NULL;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (widget), -1, -1);

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

int visual_ui_widget_set_size_request (VisUIWidget *widget, int width, int height)
{
	visual_log_return_val_if_fail (widget != NULL, -1);

	widget->width = width;
	widget->height = height;

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

void *visual_ui_widget_get_private (VisUIWidget *widget)
{
	visual_log_return_val_if_fail (widget != NULL, NULL);

	return widget->priv;
}

int visual_ui_widget_set_private (VisUIWidget *widget, void *priv)
{
	visual_log_return_val_if_fail (widget != NULL, -1);

	widget->priv = priv;

	return 0;
}

VisUIWidget *visual_ui_container_new ()
{
	VisUIContainer *container;

	container = visual_mem_new0 (VisUIContainer, 1);
	VISUAL_UI_WIDGET (container)->type = VISUAL_WIDGET_TYPE_CONTAINER;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (container), -1, -1);

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

VisUIWidget *visual_ui_box_new (VisUIOrientType orient)
{
	VisUIBox *box;

	box = visual_mem_new0 (VisUIBox, 1);
	VISUAL_UI_WIDGET (box)->type = VISUAL_WIDGET_TYPE_BOX;

	box->orient = orient;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (box), -1, -1);

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

VisUIOrientType visual_ui_box_get_orient (VisUIBox *box)
{
	visual_log_return_val_if_fail (box != NULL, VISUAL_ORIENT_TYPE_NONE);

	return box->orient;
}

VisUIWidget *visual_ui_frame_new (const char *name)
{
	VisUIFrame *frame;

	frame = visual_mem_new0 (VisUIFrame, 1);
	VISUAL_UI_WIDGET (frame)->type = VISUAL_WIDGET_TYPE_FRAME;

	frame->name = name;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (frame), -1, -1);
	
	return VISUAL_UI_WIDGET (frame);
}

VisUIWidget *visual_ui_label_new (const char *text)
{
	VisUILabel *label;

	label = visual_mem_new0 (VisUILabel, 1);
	VISUAL_UI_WIDGET (label)->type = VISUAL_WIDGET_TYPE_LABEL;

	label->text = text;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (label), -1, -1);
	
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

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (image), -1, -1);

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

VisUIWidget *visual_ui_separator_new (VisUIOrientType orient)
{
	VisUISeparator *separator;

	separator = visual_mem_new0 (VisUISeparator, 1);
	VISUAL_UI_WIDGET (separator)->type = VISUAL_WIDGET_TYPE_SEPARATOR;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (separator), -1, -1);

	return VISUAL_UI_WIDGET (separator);
}

VisUIOrientType visual_ui_separator_get_orient (VisUISeparator *separator)
{
	visual_log_return_val_if_fail (separator != NULL, VISUAL_ORIENT_TYPE_NONE);

	return separator->orient;
}

VisUIWidget *visual_ui_mutator_new ()
{
	VisUIMutator *mutator;

	mutator = visual_mem_new0 (VisUIMutator, 1);
	VISUAL_UI_WIDGET (mutator)->type = VISUAL_WIDGET_TYPE_MUTATOR;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (mutator), -1, -1);

	return VISUAL_UI_WIDGET (mutator);
}

int visual_ui_mutator_set_param (VisUIMutator *mutator, const VisParamEntry *param)
{
	visual_log_return_val_if_fail (mutator != NULL, -1);
	visual_log_return_val_if_fail (param != NULL, -1);

	/* FIXME Check if param is valid with mutator type, if not, give a critical */

	mutator->param = param;

	return 0;
}

const VisParamEntry *visual_ui_mutator_get_param (VisUIMutator *mutator)
{
	visual_log_return_val_if_fail (mutator != NULL, NULL);

	return mutator->param;
}

int visual_ui_range_set_max (VisUIRange *range, double max)
{
	visual_log_return_val_if_fail (range != NULL, -1);

	range->max = max;

	return 0;
}

int visual_ui_range_set_min (VisUIRange *range, double min)
{
	visual_log_return_val_if_fail (range != NULL, -1);

	range->min = min;

	return 0;
}

int visual_ui_range_set_step (VisUIRange *range, double step)
{
	visual_log_return_val_if_fail (range != NULL, -1);

	range->step = step;

	return 0;
}

int visual_ui_range_set_precision (VisUIRange *range, int precision)
{
	visual_log_return_val_if_fail (range != NULL, -1);

	range->precision = precision;

	return 0;
}

VisUIWidget *visual_ui_entry_new ()
{
	VisUIEntry *entry;

	entry = visual_mem_new0 (VisUIEntry, 1);
	VISUAL_UI_WIDGET (entry)->type = VISUAL_WIDGET_TYPE_ENTRY;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (entry), -1, -1);

	return VISUAL_UI_WIDGET (entry);
}

int visual_ui_entry_set_length (VisUIEntry *entry, int length)
{
	visual_log_return_val_if_fail (entry != NULL, -1);

	entry->length = length;

	return 0;
}

VisUIWidget *visual_ui_slider_new ()
{
	VisUISlider *slider;

	slider = visual_mem_new0 (VisUISlider, 1);
	VISUAL_UI_WIDGET (slider)->type = VISUAL_WIDGET_TYPE_SLIDER;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (slider), -1, -1);

	return VISUAL_UI_WIDGET (slider);
}

VisUIWidget *visual_ui_numeric_new ()
{
	VisUINumeric *numeric;

	numeric = visual_mem_new0 (VisUINumeric, 1);
	VISUAL_UI_WIDGET (numeric)->type = VISUAL_WIDGET_TYPE_NUMERIC;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (numeric), -1, -1);

	return VISUAL_UI_WIDGET (numeric);
}

VisUIWidget *visual_ui_color_new ()
{
	VisUIColor *color;

	color = visual_mem_new0 (VisUIColor, 1);
	VISUAL_UI_WIDGET (color)->type = VISUAL_WIDGET_TYPE_COLOR;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (color), -1, -1);

	return VISUAL_UI_WIDGET (color);
}

VisUIWidget *visual_ui_choice_new ()
{
	VisUIChoice *choice;

	choice = visual_mem_new0 (VisUIChoice, 1);
	VISUAL_UI_WIDGET (choice)->type = VISUAL_WIDGET_TYPE_CHOICE;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (choice), -1, -1);

	return VISUAL_UI_WIDGET (choice);
}

int visual_ui_choice_add (VisUIChoice *choice, const char *name, const VisParamEntry *value)
{
	VisUIChoiceEntry *centry;

	visual_log_return_val_if_fail (choice != NULL, -1);
	visual_log_return_val_if_fail (name != NULL, -1);
	visual_log_return_val_if_fail (value != NULL, -1);

	centry = visual_mem_new0 (VisUIChoiceEntry, 1);

	centry->name = name;
	centry->value = value;

	choice->choices.count++;
	/* FIXME be aware on object destroy, that this needs to be destroyed as well, watch out!! */
	visual_list_add (&choice->choices.choices, centry);

	return 0;
}

VisUIChoiceList *visual_ui_choice_get_choices (VisUIChoice *choice)
{
	visual_log_return_val_if_fail (choice != NULL, NULL);

	return &choice->choices;
}

VisUIWidget *visual_ui_popup_new ()
{
	VisUIPopup *popup;

	popup = visual_mem_new0 (VisUIPopup, 1);
	VISUAL_UI_WIDGET (popup)->type = VISUAL_WIDGET_TYPE_POPUP;

	VISUAL_UI_CHOICE (popup)->choices.type = VISUAL_CHOICE_TYPE_SINGLE;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (popup), -1, -1);

	return VISUAL_UI_WIDGET (popup);
}

VisUIWidget *visual_ui_list_new ()
{
	VisUIList *list;

	list = visual_mem_new0 (VisUIList, 1);
	VISUAL_UI_WIDGET (list)->type = VISUAL_WIDGET_TYPE_LIST;

	VISUAL_UI_CHOICE (list)->choices.type = VISUAL_CHOICE_TYPE_SINGLE;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (list), -1, -1);

	return VISUAL_UI_WIDGET (list);
}

VisUIWidget *visual_ui_radio_new (VisUIOrientType orient)
{
	VisUIRadio *radio;

	radio = visual_mem_new0 (VisUIRadio, 1);
	VISUAL_UI_WIDGET (radio)->type = VISUAL_WIDGET_TYPE_RADIO;

	VISUAL_UI_CHOICE (radio)->choices.type = VISUAL_CHOICE_TYPE_SINGLE;

	radio->orient = orient;
	
	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (radio), -1, -1);

	return VISUAL_UI_WIDGET (radio);
}

VisUIWidget *visual_ui_checkbox_new (const char *name)
{
	VisUICheckbox *checkbox;

	checkbox = visual_mem_new0 (VisUICheckbox, 1);
	VISUAL_UI_WIDGET (checkbox)->type = VISUAL_WIDGET_TYPE_CHECKBOX;

	VISUAL_UI_CHOICE (checkbox)->choices.type = VISUAL_CHOICE_TYPE_SINGLE;

	checkbox->name = name;

	visual_ui_widget_set_size_request (VISUAL_UI_WIDGET (checkbox), -1, -1);

	return VISUAL_UI_WIDGET (checkbox);
}

/**
 * @}
 */

