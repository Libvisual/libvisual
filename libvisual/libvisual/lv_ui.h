#ifndef _LV_UI_H
#define _LV_UI_H

#include <libvisual/lv_list.h>
#include <libvisual/lv_param.h>
#include <libvisual/lv_video.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Casting defines */
/** @todo FIXME add type check function support */
#define VISUAL_UI_CHECK_CAST(uiobj, cast_type, cast)	((cast*) (uiobj))

#define VISUAL_UI_WIDGET(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_WIDGET, VisUIWidget))
#define VISUAL_UI_CONTAINER(obj)			(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CONTAINER, VisUIContainer))
#define VISUAL_UI_BOX(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_BOX, VisUIBox))
#define VISUAL_UI_FRAME(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_FRAME, VisUIFrame))
#define VISUAL_UI_LABEL(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_LABEL, VisUILabel))
#define VISUAL_UI_IMAGE(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_IMAGE, VisUIImage))
#define VISUAL_UI_MUTATOR(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_MUTATOR, VisUIMutator))
#define VISUAL_UI_ENTRY(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_ENTRY, VisUIEntry))
#define VISUAL_UI_SLIDER(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_SLIDER, VisUISlider))
#define VISUAL_UI_NUMERIC(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_NUMERIC, VisUINumeric))
#define VISUAL_UI_COLOR(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_COLOR, VisUIColor))
#define VISUAL_UI_CHOICE(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CHOICE, VisUIChoice))
#define VISUAL_UI_POPUP(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_POPUP, VisUIPopup))
#define VISUAL_UI_LIST(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_LIST, VisUIList))
#define VISUAL_UI_RADIO(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_RADIO, VisUIRadio))
#define VISUAL_UI_CHECKBOX(obj)				(VISUAL_UI_CHECK_CAST ((obj), VISUAL_WIDGET_TYPE_CHECKBOX, VisUICheckbox))

/* FIXME add _IS_ defines */
	
typedef enum {
	VISUAL_WIDGET_TYPE_NULL,
	VISUAL_WIDGET_TYPE_WIDGET,
	VISUAL_WIDGET_TYPE_CONTAINER,
	VISUAL_WIDGET_TYPE_BOX,
	VISUAL_WIDGET_TYPE_FRAME,
	VISUAL_WIDGET_TYPE_LABEL,
	VISUAL_WIDGET_TYPE_IMAGE,
	VISUAL_WIDGET_TYPE_MUTATOR,
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
	VISUAL_BOX_TYPE_NONE,
	VISUAL_BOX_TYPE_HORIZONTAL,
	VISUAL_BOX_TYPE_VERTICAL
} VisUIBoxType;

typedef enum {
	VISUAL_CHOICE_TYPE_SINGLE,
	VISUAL_CHOICE_TYPE_MULTIPLE
} VisUIChoiceType;


typedef struct _VisUIWidget VisUIWidget;
typedef struct _VisUIContainer VisUIContainer;
typedef struct _VisUIBox VisUIBox;
typedef struct _VisUIFrame VisUIFrame;
typedef struct _VisUILabel VisUILabel;
typedef struct _VisUIImage VisUIImage;
typedef struct _VisUIMutator VisUIMutator;
typedef struct _VisUIEntry VisUIEntry;
typedef struct _VisUISlider VisUISlider;
typedef struct _VisUINumeric VisUINumeric;
typedef struct _VisUIColor VisUIColor;
typedef struct _VisUIChoice VisUIChoice;
typedef struct _VisUIChoiceList VisUIChoiceList;
typedef struct _VisUIChoiceEntry VisUIChoiceEntry;
typedef struct _VisUIPopup VisUIPopup;
typedef struct _VisUIList VisUIList;
typedef struct _VisUIRadio VisUIRadio;
typedef struct _VisUICheckbox VisUICheckbox;


struct _VisUIWidget {
	VisUIWidget		*parent;
	
	VisUIWidgetType		 type;

	void			*priv;

	int			 visible : 1;
	int			 sensitive : 1;
};

struct _VisUIContainer {
	VisUIWidget		 widget;

	VisUIWidget		*child;
};

struct _VisUIBox {
	VisUIContainer		 container;

	VisUIBoxType		 boxtype;

	VisList			 childs;
};

struct _VisUIFrame {
	VisUIContainer		 container;

	const char		*name;
};

struct _VisUILabel {
	VisUIWidget		 widget;

	const char		*text;
};

struct _VisUIImage {
	VisUIWidget		 widget;

	const VisVideo		*image;
};

struct _VisUIMutator {
	VisUIWidget		 widget;

	const VisParamEntry	*param;

	/* Numeric mutator settings */
	double			 min;
	double			 max;
	double			 step;
};

struct _VisUIEntry {
	VisUIMutator		 mutator;

	int			 length;
};

struct _VisUISlider {
	VisUIMutator		 mutator;
};

struct _VisUINumeric {
	VisUIMutator		 mutator;
};

struct _VisUIColor {
	VisUIMutator		 mutator;
};

struct _VisUIChoice {
	VisUIWidget		 widget;

	/* FIXME make non pointer */
	VisUIChoiceList		*choices;
};

struct _VisUIChoiceList {
	VisUIChoiceType		 type;

	VisList			 choices;
};

struct _VIsUIChoiceEntry {
	VisParamEntry		*param;
	VisParamEntry		*value;
};

struct _VisUIPopup {
	VisUIChoice		 choice;
};

struct _VisUIList {
	VisUIChoice		 choice;
};

struct _VisUIRadio {
	VisUIChoice		 choice;
};

struct _VisUICheckbox {
	VisUIChoice		 choice;
};

/* prototypes */
VisUIWidget *visual_ui_widget_new (void);
int visual_ui_widget_free (VisUIWidget *widget);
int visual_ui_widget_destroy (VisUIWidget *widget);
VisUIWidget *visual_ui_widget_get_top (VisUIWidget *widget);
VisUIWidget *visual_ui_widget_get_parent (VisUIWidget *widget);
VisUIWidgetType visual_ui_widget_get_type (VisUIWidget *widget);
void *visual_ui_widget_get_private (VisUIWidget *widget);
int visual_ui_widget_set_private (VisUIWidget *widget, void *priv);

VisUIWidget *visual_ui_container_new (void);
int visual_ui_container_add (VisUIContainer *container, VisUIWidget *widget);
VisUIWidget *visual_ui_container_get_child (VisUIContainer *container);

VisUIWidget *visual_ui_box_new (VisUIBoxType boxtype);
int visual_ui_box_pack (VisUIBox *box, VisUIWidget *widget);
VisUIWidget *visual_ui_box_get_next (VisUIBox *box, VisUIWidget *widget);
VisUIBoxType visual_ui_box_get_type (VisUIBox *box);

VisUIWidget *visual_ui_frame_new (const char *name);

VisUIWidget *visual_ui_label_new (const char *text);
int visual_ui_label_set_text (VisUILabel *label, const char *text);
const char *visual_ui_label_get_text (VisUILabel *label);

VisUIWidget *visual_ui_image_new (const VisVideo *video);
int visual_ui_image_set_video (VisUIImage *image, const VisVideo *video);
const VisVideo *visual_ui_image_get_video (VisUIImage *image);

VisUIWidget *visual_ui_mutator_new (void);
int visual_ui_mutator_set_param (VisUIMutator *mutator, const VisParamEntry *param);
const VisParamEntry *visual_ui_mutator_get_param (VisUIMutator *mutator);
int visual_ui_mutator_set_max (VisUIMutator *mutator, double max);
int visual_ui_mutator_set_min (VisUIMutator *mutator, double min);
int visual_ui_mutator_set_step (VisUIMutator *mutator, double step);

VisUIWidget *visual_ui_entry_new (void);
int visual_ui_entry_set_length (VisUIEntry *entry, int length);

VisUIWidget *visual_ui_slider_new (void);

VisUIWidget *visual_ui_numeric_new (void);

VisUIWidget *visual_ui_color_new (void);

VisUIWidget *visual_ui_choice_new (void);
/* FIXME helper functions, like list, entry create */

VisUIWidget *visual_ui_popup_new (void);

VisUIWidget *visual_ui_list_new (void);

VisUIWidget *visual_ui_radio_new (void);

VisUIWidget *visual_ui_checkbox_new (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_UI_H */
