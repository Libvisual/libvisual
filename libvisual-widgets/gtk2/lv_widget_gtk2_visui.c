/* Libvisual-widgets - The standard widgets for libvisual.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Milosz Derezynski <md@relevantive.de>
 * 
 * $Id: lv_widget_gtk2_visui.c,v 1.11 2006-09-19 18:43:41 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <gtk/gtksignal.h>
#include <gtk/gtktable.h>
#include <gtk/gtktogglebutton.h>

#include <libvisual/libvisual.h>

#include "lv_widget_gtk2_visui.h"

/* FIXME Look how we can make sure there are no idle callbacks pending when destroying the widget, one solution
 * could be one big idle handle multiplexer that reads a flag */
/* FIXME HIGFY everything!!! */
/* FIXME unreg param callbacks on destroy */
/* FIXME set radio buttons / popup right on init */

#if HAVE_GTK_AT_LEAST_2_4_X
#define LVW_VISUI_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LVW_VISUI_TYPE, LvwVisUIPrivate))
#else
#define LVW_VISUI_GET_PRIVATE(object) ( (LvwVisUIPrivate*) (((struct _LvwVisUI*)(object))->priv) )
#endif

struct _LvwVisUIPrivate {
	VisUIWidget	*vuitree;
	GtkTooltips	*tooltips;
	GSList		*callbacksreg;
	GdkWindow	*event_window;
	gboolean	 destroyed;
};

typedef struct _PrivIdleData PrivIdleData;
typedef struct _CallbackEntry CallbackEntry;

struct _PrivIdleData {
	VisParamEntry		*param;
	void			*priv;
};

struct _CallbackEntry {
	VisParamEntry		*param;
	int			 id;
};

static void lvwidget_visui_destroy (GtkObject *object);
static void lvwidget_visui_class_init (LvwVisUIClass *klass);
static void lvwidget_visui_init (LvwVisUI *vuic);

/* VisUI -> Gtk2 translation functions */
static GtkWidget *visui_widget_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_box_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_table_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_notebook_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_frame_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_label_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_image_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_separator_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_entry_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_slider_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_numeric_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_color_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_colorbutton_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_popup_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_radio_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);
static GtkWidget *visui_widget_checkbox_new (LvwVisUI *vuic, VisUIWidget *visuiwidget);

/* Parameter change callbacks from within GTK */
static void cb_visui_entry (GtkEditable *editable, gpointer user_data);
static void cb_visui_slider (GtkRange *range, gpointer user_data);
static void cb_visui_numeric (GtkEditable *editable, gpointer user_data);
static void cb_visui_color (GtkColorSelection *colorselection, gpointer user_data);
static void cb_visui_colorbutton (GtkColorButton *colorselection, gpointer user_data);
#if HAVE_GTK_AT_LEAST_2_4_X
static void cb_visui_popup (GtkComboBox *combobox, gpointer user_data);
#else
static void cb_visui_popup (GtkCheckMenuItem *checkmenuitem, gpointer user_data);
#endif
static void cb_visui_radio (GtkToggleButton *radiobutton, gpointer user_data);
static void cb_visui_checkbox (GtkToggleButton *togglebutton, gpointer user_data);

/* Parameter change callbacks from within VisParam */
static void cb_param_entry (VisParamEntry *param, void *priv);
static gboolean cb_idle_entry (void *userdata);

static void cb_param_slider (VisParamEntry *param, void *priv);
static gboolean cb_idle_slider (void *userdata);

static void cb_param_numeric (VisParamEntry *param, void *priv);
static gboolean cb_idle_numeric (void *userdata);

static void cb_param_color (VisParamEntry *param, void *priv);
static gboolean cb_idle_color (void *userdata);

static void cb_param_colorbutton (VisParamEntry *param, void *priv);
static gboolean cb_idle_colorbutton (void *userdata);

static void cb_param_popup (VisParamEntry *param, void *priv);
static gboolean cb_idle_popup (void *userdata);

static void cb_param_radio (VisParamEntry *param, void *priv);
static gboolean cb_idle_radio (void *userdata);

static void cb_param_checkbox (VisParamEntry *param, void *priv);
static gboolean cb_idle_checkbox (void *userdata);
	
#if (!HAVE_GTK_AT_LEAST_2_4_X)

/* Yes, we need to define this ourself... */

#define G_DEFINE_TYPE(TN, t_n, T_P)                         G_DEFINE_TYPE_EXTENDED (TN, t_n, T_P, 0, {})

#define G_DEFINE_TYPE_EXTENDED(TypeName, type_name, TYPE_PARENT, flags, CODE) \
\
static void     type_name##_init              (TypeName        *self); \
static void     type_name##_class_init        (TypeName##Class *klass); \
static gpointer type_name##_parent_class = NULL; \
static void     type_name##_class_intern_init (gpointer klass) \
{ \
  type_name##_parent_class = g_type_class_peek_parent (klass); \
  type_name##_class_init ((TypeName##Class*) klass); \
} \
\
GType \
type_name##_get_type (void) \
{ \
  static GType g_define_type_id = 0; \
  if (G_UNLIKELY (g_define_type_id == 0)) \
    { \
      static const GTypeInfo g_define_type_info = { \
        sizeof (TypeName##Class), \
        (GBaseInitFunc) NULL, \
        (GBaseFinalizeFunc) NULL, \
        (GClassInitFunc) type_name##_class_intern_init, \
        (GClassFinalizeFunc) NULL, \
        NULL,   /* class_data */ \
        sizeof (TypeName), \
        0,      /* n_preallocs */ \
        (GInstanceInitFunc) type_name##_init, \
      }; \
      g_define_type_id = g_type_register_static (TYPE_PARENT, #TypeName, &g_define_type_info, (GTypeFlags) flags); \
      { CODE ; } \
    } \
  return g_define_type_id; \
}
#endif

G_DEFINE_TYPE (LvwVisUI, lvwidget_visui, GTK_TYPE_BIN)

static void 
lvwidget_visui_destroy (GtkObject *object)
{
	GSList *head;
	CallbackEntry *cbentry;
	GtkObjectClass *klass;
	GtkObjectClass *parent_class;
	LvwVisUIPrivate *priv;

	g_return_if_fail (IS_LVW_VISUI (object));

	/* FIXME following scenario:
	 * We are here, we haven't yet locked ourself.
	 *
	 * After this function the widget will be history.
	 *
	 * Well, but in the render thread
	 * a param could change
	 * queue an idle cb
	 *
	 * AND RUN THAT AFTER WE'RE DESTROYED.. aka major problem in the sync.
	 */

	priv = LVW_VISUI_GET_PRIVATE (object);

	g_assert (priv != NULL);

	head = priv->callbacksreg;

	if (priv->destroyed == FALSE && head != NULL) {
		CallbackEntry *cbentry;
		GSList *lentry = head;
		GSList *ltmp;

		/* FIXME lock the damn thread on cb unreg */
		do {
			cbentry = lentry->data;

			visual_param_entry_remove_callback (cbentry->param, cbentry->id);

			ltmp = g_slist_next (lentry);
			head = g_slist_delete_link (head, lentry);
			lentry = ltmp;

		} while (lentry != NULL) ;
	}

	/* We don't need it anylonger, unref the VisUIWidget */
	if (priv->vuitree != NULL)
		visual_object_unref (VISUAL_OBJECT (priv->vuitree));

	priv->vuitree = NULL;

	if (priv->tooltips != NULL)
		g_object_unref (G_OBJECT (priv->tooltips));

	priv->tooltips = NULL;

	priv->destroyed = TRUE;

	klass = LVW_VISUI_CLASS (g_type_class_peek (LVW_VISUI_TYPE));
	parent_class = GTK_OBJECT_CLASS (g_type_class_peek_parent (klass));

	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(*GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
lvwidget_visui_size_allocate (GtkWidget *widget,
		GtkAllocation *allocation)
{
	LvwVisUI *lvwuic = LVW_VISUI (widget);
	GtkAllocation child_allocation;

	widget->allocation = *allocation;

	if (GTK_WIDGET_REALIZED (widget))
		gdk_window_move_resize (lvwuic->priv->event_window,
				widget->allocation.x,
				widget->allocation.y,
				widget->allocation.width,
				widget->allocation.height);

	if (GTK_BIN (lvwuic)->child && GTK_WIDGET_VISIBLE (GTK_BIN (lvwuic)->child)) {
		child_allocation.x = widget->allocation.x;
		child_allocation.y = widget->allocation.y;

		child_allocation.width = MAX (1, widget->allocation.width);
		child_allocation.height = MAX (1, widget->allocation.height);

		gtk_widget_size_allocate (GTK_BIN (lvwuic)->child, &child_allocation);
	}
}

static void
lvwidget_visui_size_request (GtkWidget *widget,
		GtkRequisition *requisition)
{ 
	LvwVisUI *lvwuic = LVW_VISUI (widget);
	GtkBorder default_border;
	gint focus_width; 
	gint focus_pad; 

	requisition->width = (GTK_CONTAINER (widget)->border_width);
	requisition->height = (GTK_CONTAINER (widget)->border_width);

	if (GTK_BIN (lvwuic)->child && GTK_WIDGET_VISIBLE (GTK_BIN (lvwuic)->child)) {
		GtkRequisition child_requisition;

		gtk_widget_size_request (GTK_BIN (lvwuic)->child, &child_requisition);

		requisition->width += child_requisition.width;
		requisition->height += child_requisition.height;
	}
}

static void
lvwidget_visui_realize (GtkWidget *widget)
{
	LvwVisUI *lvwuic;
	GdkWindowAttr attributes;
	gint attributes_mask;
	gint border_width;

	lvwuic = LVW_VISUI (widget);
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

	border_width = GTK_CONTAINER (widget)->border_width;

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x + border_width;
	attributes.y = widget->allocation.y + border_width; 
	attributes.width = widget->allocation.width - border_width * 2;
	attributes.height = widget->allocation.height - border_width * 2;
	attributes.wclass = GDK_INPUT_ONLY;
	attributes.event_mask = gtk_widget_get_events (widget);
	attributes.event_mask |= (GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK |
			GDK_ENTER_NOTIFY_MASK |
			GDK_LEAVE_NOTIFY_MASK);

	attributes_mask = GDK_WA_X | GDK_WA_Y;

	widget->window = gtk_widget_get_parent_window (widget);
	g_object_ref (widget->window);

	lvwuic->priv->event_window = gdk_window_new (gtk_widget_get_parent_window (widget),
			&attributes, attributes_mask);
	gdk_window_set_user_data (lvwuic->priv->event_window, lvwuic);

	widget->style = gtk_style_attach (widget->style, widget->window);
}

static void
lvwidget_visui_class_init (LvwVisUIClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->destroy = lvwidget_visui_destroy;
	
	widget_class->realize = lvwidget_visui_realize;
	widget_class->size_allocate = lvwidget_visui_size_allocate;
	widget_class->size_request = lvwidget_visui_size_request;

#if HAVE_GTK_AT_LEAST_2_4_X
	g_type_class_add_private (gobject_class, sizeof (LvwVisUIPrivate));
#endif
}

static void
lvwidget_visui_init (LvwVisUI *vuic)
{
	g_return_if_fail (vuic != NULL);

	vuic->priv = LVW_VISUI_GET_PRIVATE (vuic);

	GTK_WIDGET_SET_FLAGS (vuic, GTK_NO_WINDOW);
}

/**
 * @defgroup LvwVisUI LvwVisUI
 * @{
 */

/**
 * Creates a GtkWidget from a VisUIWidget. This is essentially the GTK2 drive for VisUIWidget, that means
 * that you can generate native config dialogs using this function.
 *
 * @param vuitree Pointer to the VisUIWidget that contains the user interface that needs to be converted to a
 * 	native GTK2 version.
 *
 * @return A GtkWidget containing the in the VisUIWidget described user interface.
 */
GtkWidget*
lvwidget_visui_new (VisUIWidget *vuitree)
{
	GtkWidget *widget;
	LvwVisUI *vuic;

	/* Ref it so it won't disappear under our feets */
	visual_object_ref (VISUAL_OBJECT (vuitree));

	vuic = g_object_new (lvwidget_visui_get_type (), NULL);

	/* FIXME should move into the object creation! */
#if !HAVE_GTK_AT_LEAST_2_4_X
	vuic->priv = g_new0 (LvwVisUIPrivate, 1);
#endif

	vuic->priv->callbacksreg = NULL;

	vuic->priv->vuitree = vuitree;

	vuic->priv->tooltips = gtk_tooltips_new ();

	widget = visui_widget_new (vuic, vuitree);

	gtk_container_add (GTK_CONTAINER (vuic), widget);

	gtk_widget_show_all (GTK_WIDGET (vuic));

	return GTK_WIDGET (vuic);
}

/**
 * @}
 */

static GtkWidget*
visui_widget_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	const char *tooltip = NULL;
	GtkWidget *widget;

	switch (visual_ui_widget_get_type (visuiwidget)) {
		case VISUAL_WIDGET_TYPE_BOX:
			widget = visui_widget_box_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_TABLE:
			widget = visui_widget_table_new (vuic, visuiwidget);

			break;
		
		case VISUAL_WIDGET_TYPE_NOTEBOOK:
			widget = visui_widget_notebook_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_FRAME:
			widget = visui_widget_frame_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_LABEL:
			widget = visui_widget_label_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_IMAGE:
			widget = visui_widget_image_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_SEPARATOR:
			widget = visui_widget_separator_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_ENTRY:
			widget = visui_widget_entry_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_SLIDER:
			widget = visui_widget_slider_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_NUMERIC:
			widget = visui_widget_numeric_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_COLOR:
			widget = visui_widget_color_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_COLORBUTTON:
			widget = visui_widget_colorbutton_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_POPUP:
			widget = visui_widget_popup_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_RADIO:
			widget = visui_widget_radio_new (vuic, visuiwidget);

			break;

		case VISUAL_WIDGET_TYPE_CHECKBOX:
			widget = visui_widget_checkbox_new (vuic, visuiwidget);

			break;
		
		default:
			// FIXME warning message, and an empty widget filler
			visual_log (VISUAL_LOG_CRITICAL, "Wrong VisUIWidget type, while creating gtk2 widget from VisUIWidget tree");
			widget = gtk_label_new ("");

			break;
	}

	if (visual_ui_widget_get_type (visuiwidget) != VISUAL_WIDGET_TYPE_RADIO) {
		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (visuiwidget)->width,
				VISUAL_UI_WIDGET (visuiwidget)->height);
	}

	tooltip = visual_ui_widget_get_tooltip (visuiwidget);
	if (tooltip != NULL) {
		if (GTK_WIDGET_NO_WINDOW (widget)) {
			GtkWidget *eventbox = gtk_event_box_new ();

			gtk_container_add (GTK_CONTAINER (eventbox), widget);

			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), eventbox, tooltip, tooltip);

			return eventbox;			
		} else 
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);
	}
	
	return widget;
}

static GtkWidget*
visui_widget_box_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	VisList *childs;
	VisListEntry *le = NULL;
	VisUIWidget *wi = NULL;

	if (visual_ui_box_get_orient (VISUAL_UI_BOX (visuiwidget)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
		widget = gtk_hbox_new (FALSE, 10);
	else if (visual_ui_box_get_orient (VISUAL_UI_BOX (visuiwidget)) == VISUAL_ORIENT_TYPE_VERTICAL)
		widget = gtk_vbox_new (FALSE, 10);
	else
		return NULL;

	childs = visual_ui_box_get_childs (VISUAL_UI_BOX (visuiwidget));

	while ((wi = visual_list_next (childs, &le)) != NULL) {
		GtkWidget *packer;

		packer = visui_widget_new (vuic, wi);

		gtk_box_pack_start (GTK_BOX (widget), packer, FALSE, FALSE, 0);
	}

	return widget;
}

static GtkWidget*
visui_widget_table_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	VisList *childs;
	VisListEntry *le = NULL;
	VisUITableEntry *tentry;

	widget = gtk_table_new (VISUAL_UI_TABLE (visuiwidget)->rows, VISUAL_UI_TABLE (visuiwidget)->cols, FALSE);

	gtk_table_set_row_spacings (GTK_TABLE(widget), 4);
	gtk_table_set_col_spacings (GTK_TABLE(widget), 4);

	childs = visual_ui_table_get_childs (VISUAL_UI_TABLE (visuiwidget));

	while ((tentry = visual_list_next (childs, &le)) != NULL) {
		GtkWidget *wi;

		wi = visui_widget_new (vuic, tentry->widget);

		gtk_table_attach_defaults (GTK_TABLE (widget), wi,
				tentry->col, tentry->col + 1, tentry->row, tentry->row + 1);

	}

	return widget;
}

static GtkWidget*
visui_widget_notebook_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	VisList *childs;
	VisList *childlabels;
	VisListEntry *le = NULL;
	VisListEntry *lelabel = NULL;
	VisUIWidget *tabwidget;
	VisUIWidget *labelwidget;

	widget = gtk_notebook_new ();

	childs = visual_ui_notebook_get_childs (VISUAL_UI_NOTEBOOK (visuiwidget));
	childlabels = visual_ui_notebook_get_childlabels (VISUAL_UI_NOTEBOOK (visuiwidget));

	while (((tabwidget = visual_list_next (childs, &le)) != NULL) &&
			((labelwidget = visual_list_next (childlabels, &lelabel)) != NULL)) {

		GtkWidget *tabwi = visui_widget_new (vuic, tabwidget);
		GtkWidget *labelwi = visui_widget_new (vuic, labelwidget);

		gtk_notebook_append_page (GTK_NOTEBOOK (widget), tabwi, labelwi);
	}

	return widget;
}

static GtkWidget*
visui_widget_frame_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	GtkWidget *child;

	widget = gtk_frame_new (VISUAL_UI_FRAME (visuiwidget)->name);

	if (VISUAL_UI_CONTAINER (visuiwidget)->child != NULL) {
		child = visui_widget_new (vuic, VISUAL_UI_CONTAINER (visuiwidget)->child);

		gtk_container_add (GTK_CONTAINER (widget), child);
	}

	return widget;
}

static GtkWidget*
visui_widget_label_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	GtkWidget *align;

	if (VISUAL_UI_LABEL (visuiwidget)->bold == FALSE)
		widget = gtk_label_new (visual_ui_label_get_text (VISUAL_UI_LABEL (visuiwidget)));
	else {
		char *temp;
		int len;

		widget = gtk_label_new (NULL);

		len = strlen (visual_ui_label_get_text (VISUAL_UI_LABEL (visuiwidget))) + 8;
		temp = g_malloc (len);
		snprintf (temp, len, "<b>%s</b>", visual_ui_label_get_text (VISUAL_UI_LABEL (visuiwidget)));

		gtk_label_set_markup (GTK_LABEL (widget), temp);

		g_free (temp);
	}

	align = gtk_alignment_new (0, 0, 0, 0);
	gtk_container_add (GTK_CONTAINER (align), widget);

	return align;
}

static GtkWidget*
visui_widget_image_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	GdkPixbuf *pixbuf;
	GdkPixmap *pixmap;
	GdkDrawable *drawable;
	GdkColormap *colormap;
	GdkGC *gc;
	const VisVideo *video;
	unsigned char *buf;
	int x, y;
	int i = 0;

	video = visual_ui_image_get_video (VISUAL_UI_IMAGE (visuiwidget));

	if (video == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Image widget doesn't contain an image");

		return NULL;
	}

	if (video->depth != VISUAL_VIDEO_DEPTH_24BIT) {
		visual_log (VISUAL_LOG_CRITICAL, "Image in image widget must be VISUAL_VIDEO_DEPTH_24BIT");

		return NULL;
	}

	pixmap = gdk_pixmap_new (NULL, video->width, video->height, 24);
	drawable = pixmap;

	colormap = gdk_colormap_get_system ();
	gdk_drawable_set_colormap (drawable, colormap);

	gc = gdk_gc_new (drawable);


	buf = visual_video_get_pixels (video);
	for (y = 0; y < video->height; y++) {
		for (x = 0; x < video->width; x++) {
			GdkColor color;

			color.red = buf[i++] * (65535 / 255);
			color.green = buf[i++] * (65535 / 255);
			color.blue = buf[i++] * (65535 / 255);

			gdk_gc_set_rgb_fg_color (gc, &color);
			gdk_draw_point (drawable, gc, x, y);
		}
	}

	/* FIXME Must we free this or not ? */
	pixbuf = gdk_pixbuf_get_from_drawable (NULL, drawable, colormap, 0, 0, 0, 0, video->width, video->height);		

	widget = gtk_image_new_from_pixbuf (pixbuf);

	return widget;
}

static GtkWidget*
visui_widget_separator_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	
	if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (visuiwidget)) == VISUAL_ORIENT_TYPE_NONE) {
		visual_log (VISUAL_LOG_CRITICAL, "Separator orientation must be HORIZONTAL or VERTICAL");

		return NULL;
	}

	if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (visuiwidget)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
		widget = gtk_hseparator_new ();
	else
		widget = gtk_vseparator_new ();

	return widget;
}

static GtkWidget*
visui_widget_entry_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;

	widget = gtk_entry_new ();

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	gtk_entry_set_text (GTK_ENTRY (widget), visual_param_entry_get_string (param));
	gtk_entry_set_max_length (GTK_ENTRY (widget), VISUAL_UI_ENTRY (visuiwidget)->length);

	g_signal_connect (G_OBJECT (widget), "changed",
			G_CALLBACK (cb_visui_entry), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_entry, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_slider_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	double val;

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);
	else {
		visual_log (VISUAL_LOG_CRITICAL, "Param for numeric widget must be of numeric type");

		return NULL;
	}

	widget = gtk_hscale_new_with_range (VISUAL_UI_RANGE (visuiwidget)->min,
			VISUAL_UI_RANGE (visuiwidget)->max,
			VISUAL_UI_RANGE (visuiwidget)->step);

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	if (VISUAL_UI_SLIDER (visuiwidget)->showvalue == FALSE)
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
	else
		gtk_scale_set_draw_value (GTK_SCALE (widget), TRUE);

	gtk_scale_set_digits (GTK_SCALE (widget), VISUAL_UI_RANGE (visuiwidget)->precision);

	gtk_range_set_value (GTK_RANGE (widget), val); 

	g_signal_connect (G_OBJECT (widget), "value-changed",
			G_CALLBACK (cb_visui_slider), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_slider, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_numeric_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	double val;

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);
	else {
		visual_log (VISUAL_LOG_CRITICAL, "Param for numeric widget must be of numeric type");

		return NULL;
	}

	widget = gtk_spin_button_new_with_range (VISUAL_UI_RANGE (visuiwidget)->min,
			VISUAL_UI_RANGE (visuiwidget)->max,
			VISUAL_UI_RANGE (visuiwidget)->step);

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (widget), TRUE);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (widget), VISUAL_UI_RANGE (visuiwidget)->precision);

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), val); 

	g_signal_connect (G_OBJECT (widget), "changed",
			G_CALLBACK (cb_visui_numeric), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_numeric, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_color_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	VisColor *color;
	GdkColor gdkcol;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
		visual_log (VISUAL_LOG_CRITICAL, "Param for color widget must be of color type");

		return NULL;
	}

	color = visual_param_entry_get_color (param);

	gdkcol.red = color->r * (65535 / 255);
	gdkcol.blue = color->b * (65535 / 255);
	gdkcol.green = color->g * (65535 / 255);

	widget = gtk_color_selection_new ();

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (widget), &gdkcol);

	g_signal_connect (G_OBJECT (widget), "color-changed",
			G_CALLBACK (cb_visui_color), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_color, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_colorbutton_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	VisColor *color;
	GdkColor gdkcol;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
		visual_log (VISUAL_LOG_CRITICAL, "Param for colorbutton widget must be of color type");

		return NULL;
	}

	color = visual_param_entry_get_color (param);

	gdkcol.red = color->r * (65535 / 255);
	gdkcol.blue = color->b * (65535 / 255);
	gdkcol.green = color->g * (65535 / 255);

	widget = gtk_color_button_new_with_color (&gdkcol);

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	g_signal_connect (G_OBJECT (widget), "color-set",
			G_CALLBACK (cb_visui_colorbutton), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_colorbutton, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_popup_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	VisUIChoiceList *options;
	VisUIChoiceEntry *centry;
	VisListEntry *le = NULL;
	VisList *choices;
	GtkWidget *menu, *menuitem;
	GSList *group;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (visuiwidget));
	choices = &options->choices;

#if HAVE_GTK_AT_LEAST_2_4_X
	widget = gtk_combo_box_new_text ();

	while ((centry = visual_list_next (choices, &le)) != NULL)
		gtk_combo_box_append_text (GTK_COMBO_BOX (widget), centry->name);

	gtk_combo_box_set_active (GTK_COMBO_BOX (widget), visual_ui_choice_get_active (VISUAL_UI_CHOICE (visuiwidget)));

	g_signal_connect (G_OBJECT (widget), "changed",
			G_CALLBACK (cb_visui_popup), visuiwidget);
#else
	widget = gtk_option_menu_new ();

	menu = gtk_menu_new ();
	group = NULL;

	while ((centry = visual_list_next (choices, &le)) != NULL) {
		menuitem = gtk_radio_menu_item_new_with_label (group, centry->name);
		group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM(menuitem));
		gtk_menu_append (menu, menuitem);
		gtk_widget_show (menuitem);

		g_signal_connect (G_OBJECT (menuitem), "toggled",
				G_CALLBACK (cb_visui_popup), visuiwidget);
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (widget), menu);
#endif
	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_popup, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_radio_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	GtkWidget *radio;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	VisUIChoiceList *options;
	VisUIChoiceEntry *centry;
	VisListEntry *le = NULL;
	VisList *choices;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (visuiwidget));
	choices = &options->choices;

	if (options->count < 2) {
		visual_log (VISUAL_LOG_CRITICAL, "The number of choices for a radio button must be atleast 2");

		return NULL;
	}

	if (VISUAL_UI_RADIO (visuiwidget)->orient == VISUAL_ORIENT_TYPE_HORIZONTAL)
		widget = gtk_hbox_new (FALSE, 10);
	else
		widget = gtk_vbox_new (FALSE, 10);

	/* First entry */
	centry = visual_list_next (choices, &le);
	radio = gtk_radio_button_new_with_label (NULL, centry->name);

	gtk_widget_set_size_request (GTK_WIDGET (radio),
			VISUAL_UI_WIDGET (visuiwidget)->width,
			VISUAL_UI_WIDGET (visuiwidget)->height);

	g_signal_connect (G_OBJECT (radio), "toggled",
			G_CALLBACK (cb_visui_radio), visuiwidget);

	gtk_box_pack_start (GTK_BOX (widget), radio, FALSE, FALSE, 0);

	while ((centry = visual_list_next (choices, &le)) != NULL) {
		radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio), centry->name);

		gtk_widget_set_size_request (GTK_WIDGET (radio),
				VISUAL_UI_WIDGET (visuiwidget)->width,
				VISUAL_UI_WIDGET (visuiwidget)->height);

		g_signal_connect (G_OBJECT (radio), "toggled",
				G_CALLBACK (cb_visui_radio), visuiwidget);

		gtk_box_pack_start (GTK_BOX (widget), radio, FALSE, FALSE, 0);
	}

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_radio, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

static GtkWidget*
visui_widget_checkbox_new (LvwVisUI *vuic, VisUIWidget *visuiwidget)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisParamEntry *param;
	VisUIChoiceList *options;

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (visuiwidget));

	options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (visuiwidget));

	if (options->count != 2) {
		visual_log (VISUAL_LOG_CRITICAL, "The number of choices for a checkbox must be 2");

		return NULL;
	}

	if (VISUAL_UI_CHECKBOX (visuiwidget)->name != NULL)
		widget = gtk_check_button_new_with_label (VISUAL_UI_CHECKBOX (visuiwidget)->name);
	else
		widget = gtk_check_button_new ();

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), visual_ui_choice_get_active (VISUAL_UI_CHOICE (visuiwidget)));

	visual_object_set_private (VISUAL_OBJECT (visuiwidget), widget);

	g_signal_connect (G_OBJECT (widget), "toggled",
			G_CALLBACK (cb_visui_checkbox), visuiwidget);

	cbentry = g_new0 (CallbackEntry, 1);
	cbentry->param = param;
	cbentry->id = visual_param_entry_add_callback (param, cb_param_checkbox, visuiwidget);
	vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

	return widget;
}

/* Parameter change callbacks from within GTK */
static void
cb_visui_entry (GtkEditable *editable, gpointer user_data)
{
	VisParamEntry *param;
	char *text;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));

	text = gtk_editable_get_chars (editable, 0, -1);

	visual_param_entry_set_string (param, text);

	g_free (text);
}

static void
cb_visui_slider (GtkRange *range, gpointer user_data)
{
	VisParamEntry *param;
	gdouble value;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));

	value = gtk_range_get_value (range);

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		visual_param_entry_set_integer (param, value);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		visual_param_entry_set_float (param, value);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		visual_param_entry_set_double (param, value);
	else
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the slider isn't a numeric param");

	printf ("Changed value: %f\n", value);
}

static void
cb_visui_numeric (GtkEditable *editable, gpointer user_data)
{
	VisParamEntry *param;
	gdouble value;

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data)); 
	value = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON (editable));

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		visual_param_entry_set_integer (param, value);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		visual_param_entry_set_float (param, value);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		visual_param_entry_set_double (param, value);
	else
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the numeric isn't a numeric param");

	printf ("Changed value: %f\n", value);
}

static void
cb_visui_color (GtkColorSelection *colorselection, gpointer user_data)
{
	VisParamEntry *param;
	GdkColor color;

	gtk_color_selection_get_current_color (colorselection, &color);

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR)
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the color selector isn't a color param");

	visual_param_entry_set_color (param, color.red >> 8, color.green >> 8, color.blue >> 8);
}

static void
cb_visui_colorbutton (GtkColorButton *colorbutton, gpointer user_data)
{
	VisParamEntry *param;
	GdkColor color;

	gtk_color_button_get_color (colorbutton, &color);

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR)
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the colorbutton selector isn't a color param");

	visual_param_entry_set_color (param, color.red >> 8, color.green >> 8, color.blue >> 8);
}

#if HAVE_GTK_AT_LEAST_2_4_X
static void
cb_visui_popup (GtkComboBox *combobox, gpointer user_data)
{
	int active;
	/* FIXME remove reference to param, is for debug purpose */
	VisParamEntry *param;

	active = gtk_combo_box_get_active (combobox);

	visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), active);

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));
	
	printf ("Popup changed gtk2.4, active: %d\n", active);
}
#else
static void
cb_visui_popup (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
	GtkRadioMenuItem *radiomenuitem = GTK_RADIO_MENU_ITEM (checkmenuitem);
	int active;
	GSList *group, *l;

	/* Catch off the unactivate toggle */
	if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (checkmenuitem)) == FALSE)
		return;

	group = gtk_radio_menu_item_get_group (radiomenuitem);

	active = g_slist_length (group);
	
	l = group;
	
	while (l) {
		if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (l->data)))
			break;

		active--;
		l = g_slist_next (l);
	}

	/* The choice group indexing starts at 0, not 1 */
	active--;

	if (active < 0)
		active = 0;
	
	visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), active);

	printf ("Popup changed gtk2.x, active: %d\n", active);
}
#endif

static void
cb_visui_radio (GtkToggleButton *togglebutton, gpointer user_data)
{
	GSList *group, *temp;
	GtkRadioButton *but;
	int entries = 0;
	int cnt = 0;

	VisParamEntry *param;

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (togglebutton));

	temp = group;
	while ((temp = g_slist_next (temp)) != NULL)
		entries++;

	but = group->data;

	/* First entry is active */
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (but)) == TRUE) {
		visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), entries);
		
		param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));
		printf ("Radio changed, active: %d\n", cnt);
		return;
	}

	/* Not the first entry */
	while ((group = g_slist_next (group)) != NULL) {
		cnt++;

		but = group->data;

		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (but)) == TRUE) {
			visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), entries - cnt);

			param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));
			printf ("Radio changed, active: %d\n", cnt);
			return;
		}
	}
}

static void
cb_visui_checkbox (GtkToggleButton *togglebutton, gpointer user_data)
{
	gboolean state;
	/* FIXME remove reference to param, is for debug purpose */
	VisParamEntry *param;

	state = gtk_toggle_button_get_active (togglebutton);

	if (state == TRUE)
		visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), 1);
	else
		visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), 0);

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));
	
	printf ("Checkbox changed, active: %d\n", state);
}

/* Parameter change callbacks from within VisParam */
static void
cb_param_entry (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	printf ("Param ENTRY callback\n");

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_entry, privdata);
}

static gboolean
cb_idle_entry (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_entry_set_text (GTK_ENTRY (visual_object_get_private (VISUAL_OBJECT (widget))),
				visual_param_entry_get_string (param));

	g_free (data);

	return FALSE;
}

static void
cb_param_slider (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	printf ("Param SLIDER callback\n");

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_slider, privdata);
}

static gboolean
cb_idle_slider (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	double val = 0;

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);

	gtk_range_set_value (GTK_RANGE (visual_object_get_private (VISUAL_OBJECT (widget))),
			val); 

	g_free (data);

	return FALSE;
}

static void
cb_param_numeric (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	printf ("Param NUMERIC callback\n");

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_numeric, privdata);
}

static gboolean
cb_idle_numeric (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	GtkAdjustment *adj;
	double val = 0;

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);

	adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (visual_object_get_private (VISUAL_OBJECT (widget))));
	gtk_adjustment_set_value (adj, val);

	g_free (data);

	return FALSE;
}

static void
cb_param_color (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_color, privdata);
}

static gboolean
cb_idle_color (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	VisColor *color;
	GdkColor gdkcol;
	GdkColor testcol;

	gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (visual_object_get_private (VISUAL_OBJECT (widget))),
			&testcol);

	color = visual_param_entry_get_color (param);
	
	gdkcol.red = color->r * (65535 / 255);
	gdkcol.blue = color->b * (65535 / 255);
	gdkcol.green = color->g * (65535 / 255);

	// FIXME there is still a misterious rounding error, it's difficult to trigger, but possible, look at it! */
	
	/* Only set the color if it's really different, else it can cause a saturation change and that is ugly */
	if (!((testcol.red >> 8) == color->r && (testcol.blue >> 8) == color->b && (testcol.green >> 8) == color->g)) {
		gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (visual_object_get_private (VISUAL_OBJECT (widget))),
				&gdkcol);
	}
				
	g_free (data);

	return FALSE;
}

static void
cb_param_colorbutton (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_colorbutton, privdata);
}

static gboolean
cb_idle_colorbutton (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	VisColor *color;
	GdkColor gdkcol;
	GdkColor testcol;

	gtk_color_button_get_color (GTK_COLOR_BUTTON (visual_object_get_private (VISUAL_OBJECT (widget))),
			&testcol);

	color = visual_param_entry_get_color (param);
	
	gdkcol.red = color->r * (65535 / 255);
	gdkcol.blue = color->b * (65535 / 255);
	gdkcol.green = color->g * (65535 / 255);

	// FIXME there is still a misterious rounding error, it's difficult to trigger, but possible, look at it! */
	
	/* Only set the color if it's really different, else it can cause a saturation change and that is ugly */
	if (!((testcol.red >> 8) == color->r && (testcol.blue >> 8) == color->b && (testcol.green >> 8) == color->g)) {
		gtk_color_button_set_color (GTK_COLOR_BUTTON (visual_object_get_private (VISUAL_OBJECT (widget))),
				&gdkcol);
	}
				
	g_free (data);

	return FALSE;
}

static void
cb_param_popup (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	printf ("Param POPUP callback\n");

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_popup, privdata);
}

static gboolean
cb_idle_popup (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

#if HAVE_GTK_AT_LEAST_2_4_X
	gtk_combo_box_set_active (GTK_COMBO_BOX (visual_object_get_private (VISUAL_OBJECT (widget))),
			visual_ui_choice_get_active (widget));
#endif

	g_free (data);

	return FALSE;
}
	
static void
cb_param_radio (VisParamEntry *param, void *priv)
{
	// FIXME maken.
	printf ("Param RADIO callback\n");
}

static gboolean
cb_idle_radio (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	g_free (data);

	return FALSE;
}

static void
cb_param_checkbox (VisParamEntry *param, void *priv)
{
	PrivIdleData *privdata;

	printf ("Param CHECKBOX callback\n");

	privdata = g_new0 (PrivIdleData, 1);

	privdata->param = param;
	privdata->priv = priv;

	g_idle_add (cb_idle_checkbox, privdata);
}

static gboolean
cb_idle_checkbox (void *userdata)
{
	PrivIdleData *data = userdata;
	VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (visual_object_get_private (VISUAL_OBJECT (widget))),
			visual_ui_choice_get_active (widget));

	g_free (data);

	return FALSE;
}

