#ifndef _LV_UI_H
#define _LV_UI_H

#include <libvisual/lv_list.h>
#include <libvisual/lv_param.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_UI_WIDGET(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_WIDGET, VisUIWidget))
#define VISUAL_UI_CONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CONTAINER, VisUIContainer))
#define VISUAL_UI_BOX(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_BOX, VisUIBox))
#define VISUAL_UI_TABLE_ENTRY(obj)			(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_TABLE, VisUITableEntry))
#define VISUAL_UI_TABLE(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_TABLE, VisUITable))
#define VISUAL_UI_FRAME(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_FRAME, VisUIFrame))
#define VISUAL_UI_LABEL(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_LABEL, VisUILabel))
#define VISUAL_UI_IMAGE(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_IMAGE, VisUIImage))
#define VISUAL_UI_SEPARATOR(obj)			(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_SEPARATOR, VisUISeparator))
#define VISUAL_UI_MUTATOR(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_MUTATOR, VisUIMutator))
#define VISUAL_UI_RANGE(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_RANGE, VisUIRange))
#define VISUAL_UI_ENTRY(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_ENTRY, VisUIEntry))
#define VISUAL_UI_SLIDER(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_SLIDER, VisUISlider))
#define VISUAL_UI_NUMERIC(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_NUMERIC, VisUINumeric))
#define VISUAL_UI_COLOR(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_COLOR, VisUIColor))
#define VISUAL_UI_CHOICE_ENTRY(obj)			(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CHOICE, VisUIChoiceEntry))
#define VISUAL_UI_CHOICE(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CHOICE, VisUIChoice))
#define VISUAL_UI_POPUP(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_POPUP, VisUIPopup))
#define VISUAL_UI_LIST(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_LIST, VisUIList))
#define VISUAL_UI_RADIO(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_RADIO, VisUIRadio))
#define VISUAL_UI_CHECKBOX(obj)				(VISUAL_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CHECKBOX, VisUICheckbox))


typedef enum {
	VISUAL_WIDGET_TYPE_NULL,
	VISUAL_WIDGET_TYPE_WIDGET,
	VISUAL_WIDGET_TYPE_CONTAINER,
	VISUAL_WIDGET_TYPE_BOX,
	VISUAL_WIDGET_TYPE_TABLE,
	VISUAL_WIDGET_TYPE_FRAME,
	VISUAL_WIDGET_TYPE_LABEL,
	VISUAL_WIDGET_TYPE_IMAGE,
	VISUAL_WIDGET_TYPE_SEPARATOR,
	VISUAL_WIDGET_TYPE_MUTATOR,
	VISUAL_WIDGET_TYPE_RANGE,
	VISUAL_WIDGET_TYPE_ENTRY,
	VISUAL_WIDGET_TYPE_SLIDER,
	VISUAL_WIDGET_TYPE_NUMERIC,
	VISUAL_WIDGET_TYPE_COLOR,
	VISUAL_WIDGET_TYPE_CHOICE,
	VISUAL_WIDGET_TYPE_POPUP,
	VISUAL_WIDGET_TYPE_LIST,
	VISUAL_WIDGET_TYPE_RADIO,
	VISUAL_WIDGET_TYPE_CHECKBOX
} VisUIWidgetType;

typedef enum {
	VISUAL_ORIENT_TYPE_NONE,
	VISUAL_ORIENT_TYPE_HORIZONTAL,
	VISUAL_ORIENT_TYPE_VERTICAL
} VisUIOrientType;

typedef enum {
	VISUAL_CHOICE_TYPE_NONE,
	VISUAL_CHOICE_TYPE_SINGLE,
	VISUAL_CHOICE_TYPE_MULTIPLE
} VisUIChoiceType;

typedef struct _VisUIWidget VisUIWidget;
typedef struct _VisUIContainer VisUIContainer;
typedef struct _VisUIBox VisUIBox;
typedef struct _VisUITableEntry VisUITableEntry;
typedef struct _VisUITable VisUITable;
typedef struct _VisUIFrame VisUIFrame;
typedef struct _VisUILabel VisUILabel;
typedef struct _VisUIImage VisUIImage;
typedef struct _VisUISeparator VisUISeparator;
typedef struct _VisUIMutator VisUIMutator;
typedef struct _VisUIRange VisUIRange;
typedef struct _VisUIEntry VisUIEntry;
typedef struct _VisUISlider VisUISlider;
typedef struct _VisUINumeric VisUINumeric;
typedef struct _VisUIColor VisUIColor;
typedef struct _VisUIChoiceList VisUIChoiceList;
typedef struct _VisUIChoiceEntry VisUIChoiceEntry;
typedef struct _VisUIChoice VisUIChoice;
typedef struct _VisUIPopup VisUIPopup;
typedef struct _VisUIList VisUIList;
typedef struct _VisUIRadio VisUIRadio;
typedef struct _VisUICheckbox VisUICheckbox;


struct _VisUIWidget {
	VisObject		 object;

	VisUIWidget		*parent;
	
	VisUIWidgetType		 type;

	const char		*tooltip;

	int			 width;
	int			 height;
};

struct _VisUIContainer {
	VisUIWidget		 widget;

	VisUIWidget		*child;
};

struct _VisUIBox {
	VisUIContainer		 container;

	VisUIOrientType		 orient;

	VisList			 childs;
};

struct _VisUITableEntry {
	VisObject		object;

	int			row;
	int			col;

	VisUIWidget		*widget;

};

struct _VisUITable {
	VisUIContainer		 container;

	int			 rows;
	int			 cols;

	VisList			 childs;
};

struct _VisUIFrame {
	VisUIContainer		 container;

	const char		*name;
};

struct _VisUILabel {
	VisUIWidget		 widget;

	const char		*text;
	int			 bold;
};

struct _VisUIImage {
	VisUIWidget		 widget;

	const VisVideo		*image;
};

struct _VisUISeparator {
	VisUIWidget		 widget;

	VisUIOrientType		 orient;
};

struct _VisUIMutator {
	VisUIWidget		 widget;

	const VisParamEntry	*param;
};

struct _VisUIRange {
	VisUIMutator		 mutator;

	double			 min;
	double			 max;
	double			 step;

	int			 precision;
};

struct _VisUIEntry {
	VisUIMutator		 mutator;

	int			 length;
};

struct _VisUISlider {
	VisUIRange		 range;

	int			 showvalue;
};

struct _VisUINumeric {
	VisUIRange		 range;
};

struct _VisUIColor {
	VisUIMutator		 mutator;
};

struct _VisUIChoiceList {
	VisObject		 object;

	VisUIChoiceType		 type;

	int			 count;
	VisList			 choices;
};

struct _VisUIChoiceEntry {
	VisObject		 object;

	const char		*name;
	
	const VisParamEntry	*value;
};

struct _VisUIChoice {
	VisUIMutator		 mutator;

	const VisParamEntry	*param;

	VisUIChoiceList		 choices;
};

struct _VisUIPopup {
	VisUIChoice		 choice;
};

struct _VisUIList {
	VisUIChoice		 choice;
};

struct _VisUIRadio {
	VisUIChoice		 choice;

	VisUIOrientType		 orient;
};

struct _VisUICheckbox {
	VisUIChoice		 choice;

	const char		*name;
};

/* prototypes */
VisUIWidget *visual_ui_widget_new (void);
int visual_ui_widget_set_size_request (VisUIWidget *widget, int width, int height);
int visual_ui_widget_set_tooltip (VisUIWidget *widget, const char *tooltip);
const char *visual_ui_widget_get_tooltip (VisUIWidget *widget);
VisUIWidget *visual_ui_widget_get_top (VisUIWidget *widget);
VisUIWidget *visual_ui_widget_get_parent (VisUIWidget *widget);
VisUIWidgetType visual_ui_widget_get_type (VisUIWidget *widget);

int visual_ui_container_add (VisUIContainer *container, VisUIWidget *widget);
VisUIWidget *visual_ui_container_get_child (VisUIContainer *container);

VisUIWidget *visual_ui_box_new (VisUIOrientType orient);
int visual_ui_box_pack (VisUIBox *box, VisUIWidget *widget);
VisList *visual_ui_box_get_childs (VisUIBox *box);
VisUIOrientType visual_ui_box_get_orient (VisUIBox *box);

VisUIWidget *visual_ui_table_new (int rows, int cols);
VisUITableEntry *visual_ui_table_entry_new (VisUIWidget *widget, int row, int col);
int visual_ui_table_attach (VisUITable *table, VisUIWidget *widget, int row, int col);
VisList *visual_ui_table_get_childs (VisUITable *table);

VisUIWidget *visual_ui_frame_new (const char *name);

VisUIWidget *visual_ui_label_new (const char *text, int bold);
int visual_ui_label_set_text (VisUILabel *label, const char *text);
int visual_ui_label_set_bold (VisUILabel *label, int bold);
const char *visual_ui_label_get_text (VisUILabel *label);

VisUIWidget *visual_ui_image_new (const VisVideo *video);
int visual_ui_image_set_video (VisUIImage *image, const VisVideo *video);
const VisVideo *visual_ui_image_get_video (VisUIImage *image);

VisUIWidget *visual_ui_separator_new (VisUIOrientType orient);
VisUIOrientType visual_ui_separator_get_orient (VisUISeparator *separator);

int visual_ui_mutator_set_param (VisUIMutator *mutator, const VisParamEntry *param);
const VisParamEntry *visual_ui_mutator_get_param (VisUIMutator *mutator);

int visual_ui_range_set_properties (VisUIRange *range, double min, double max, double step, int precision);
int visual_ui_range_set_max (VisUIRange *range, double max);
int visual_ui_range_set_min (VisUIRange *range, double min);
int visual_ui_range_set_step (VisUIRange *range, double step);
int visual_ui_range_set_precision (VisUIRange *range, int precision);

VisUIWidget *visual_ui_entry_new (void);
int visual_ui_entry_set_length (VisUIEntry *entry, int length);

VisUIWidget *visual_ui_slider_new (int showvalue);

VisUIWidget *visual_ui_numeric_new (void);

VisUIWidget *visual_ui_color_new (void);

VisUIChoiceEntry *visual_ui_choice_entry_new (const char *name, const VisParamEntry *value);
int visual_ui_choice_add (VisUIChoice *choice, const char *name, const VisParamEntry *value);
int visual_ui_choice_add_many (VisUIChoice *choice, const VisParamEntry *paramchoices);
int visual_ui_choice_free_choices (VisUIChoice *choice);
int visual_ui_choice_set_active (VisUIChoice *choice, int index);
int visual_ui_choice_get_active (VisUIChoice *choice);
VisUIChoiceEntry *visual_ui_choice_get_choice (VisUIChoice *choice, int index);
VisUIChoiceList *visual_ui_choice_get_choices (VisUIChoice *choice);

/* FIXME look at lists with multiple selections... */

VisUIWidget *visual_ui_popup_new (void);

VisUIWidget *visual_ui_list_new (void);

VisUIWidget *visual_ui_radio_new (VisUIOrientType orient);

VisUIWidget *visual_ui_checkbox_new (const char *name, int boolcheck);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_UI_H */
