#ifndef _LV_UI_H
#define _LV_UI_H

#include <libvisual/lv_list.h>
#include <libvisual/lv_param.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	VISUAL_WIDGET_TYPE_WIDGET,
	VISUAL_WIDGET_TYPE_CONTAINER,
	VISUAL_WIDGET_TYPE_BOX,
	VISUAL_WIDGET_TYPE_GROUP,
	VISUAL_WIDGET_TYPE_LABEL,
	VISUAL_WIDGET_TYPE_IMAGE,
	VISUAL_WIDGET_TYPE_MUTATOR,
	VISUAL_WIDGET_TYPE_TEXT,
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
typedef struct _VisUIGroup VisUIGroup;
typedef struct _VisUILabel VisUILabel;
typedef struct _VisUIImage VisUIImage;
typedef struct _VisUIMutator VisUIMutator;
typedef struct _VisUIText VisUIText;
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
	VisUIWidgetType		 type;
	void			*child;

	int			 visible;
	int			 sensitive;
};

struct _VisUIContainer {
	VisUIWidget		*parent;

	VisUIWidgetType		 type;
	void			*child;
};

struct _VisUIBox {
	VisUIBoxType		 boxtype;
	
	VisUIContainer		*parent;

	VisList			 childs;
};

struct _VisUIGroup {
	VisUIContainer		*parent;

	VisList			 childs;
};

struct _VisUILabel {
	VisUIWidget		*parent;

	char			*text;
};

struct _VisUIImage {
	VisUIWidget		*parent;

	VisVideo		*image;
};

struct _VisUIMutator {
	VisUIWidget		*parent;

	VisUIWidgetType		 type;
	void			*child;
};

struct _VisUIText {
	VisUIMutator		*parent;
};

struct _VisUISlider {
	VisUIMutator		*parent;
};

struct _VisUINumeric {
	VisUIMutator		*parent;
};

struct _VisUIColor {
	VisUIMutator		*parent;
};

struct _VisUIChoice {
	VisUIWidget		*parent;

	VisUIWidgetType		 type;
	void			*child;

	VisUIChoiceList		*choices;
};

struct _VisUIChoiceList {
	VisUIChoiceType		 type;

	VisList			*choices;
};

struct _VIsUIChoiceEntry {
	VisParamEntry		*param;
	VisParamEntry		*value;
};

struct _VisUIPopup {
	VisUIChoice		*parent;
};

struct _VisUIList {
	VisUIChoice		*parent;
};

struct VisUIRadio {
	VisUIChoice		*parent;
};

struct VisUICheckbox {
	VisUIChoice		*parent;
};

/* prototypes */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_UI_H */
