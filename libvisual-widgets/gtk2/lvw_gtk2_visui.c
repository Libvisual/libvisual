#include <gtk/gtksignal.h>
#include <gtk/gtktable.h>
#include <gtk/gtktogglebutton.h>

#include <libvisual/libvisual.h>

#include "lvw_gtk2_visui.h"

/* FIXME Look how we can make sure there are no idle callbacks pending when destroying the widget */
/* FIXME HIGFY everything!!! */
/* FIXME unreg param callbacks on destroy */
/* FIXME implement tooltips */

typedef struct _PrivIdleData PrivIdleData;
typedef struct _CallbackEntry CallbackEntry;

struct _PrivIdleData {
	const VisParamEntry	*param;
	void			*priv;
};

struct _CallbackEntry {
	const VisParamEntry		*param;
	param_changed_callback_func_t	 callback;
};

static void lvw_visui_destroy (GtkObject *object);
static void lvw_visui_class_init (LvwVisUIClass *klass);
static void lvw_visui_init (LvwVisUI *vuic);
static GtkWidget *lvw_visui_create_gtk_widgets (LvwVisUI *vuic, VisUIWidget *cont, int cnt);

/* Parameter change callbacks from within GTK */
static void cb_visui_entry (GtkEditable *editable, gpointer user_data);
static void cb_visui_slider (GtkRange *range, gpointer user_data);
static void cb_visui_numeric (GtkEditable *editable, gpointer user_data);
static void cb_visui_color (GtkColorSelection *colorselection, gpointer user_data);
static void cb_visui_popup (GtkComboBox *combobox, gpointer user_data);
static void cb_visui_radio (GtkToggleButton *radiobutton, gpointer user_data);
static void cb_visui_checkbox (GtkToggleButton *togglebutton, gpointer user_data);

/* Parameter change callbacks from within VisParam */
static void cb_param_entry (const VisParamEntry *param, void *priv);
static gboolean cb_idle_entry (void *userdata);

static void cb_param_slider (const VisParamEntry *param, void *priv);
static gboolean cb_idle_slider (void *userdata);

static void cb_param_numeric (const VisParamEntry *param, void *priv);
static gboolean cb_idle_numeric (void *userdata);

static void cb_param_color (const VisParamEntry *param, void *priv);
static gboolean cb_idle_color (void *userdata);

static void cb_param_popup (const VisParamEntry *param, void *priv);
static gboolean cb_idle_popup (void *userdata);

static void cb_param_radio (const VisParamEntry *param, void *priv);
static gboolean cb_idle_radio (void *userdata);

static void cb_param_checkbox (const VisParamEntry *param, void *priv);
static gboolean cb_idle_checkbox (void *userdata);
	
G_DEFINE_TYPE (LvwVisUI, lvw_visui, GTK_TYPE_BIN)

static void 
lvw_visui_destroy (GtkObject *object)
{
	GSList *head;
	GSList *next;
	CallbackEntry *cbentry;
	GtkObjectClass *klass;
	GtkObjectClass *parent_class;

	g_return_if_fail (IS_LVW_VISUI (object));

#if 0
	head = LVW_VISUI (object)->callbacksreg;
	printf ("HMMMM\n");
	next = head;

	if (head == NULL)
		return;

	/* FIXME is this thread safe ??, check this out, and if not, lock here */

	/* You have to lock, this isn't thread safe, the signal handler could still
	 * be called here -- deadchip */

	do {
		cbentry = next->data;

		//		if (cbentry != NULL) {
		//			printf ("%p %s\n", cbentry, cbentry->param->name);
		//		}
		//		visual_param_entry_remove_callback (cbentry->param, cbentry->callback);
		//		g_free (cbentry);

		next = g_slist_delete_link (head, next);
	} while ((next = g_slist_next (next)) != NULL);

	LVW_VISUI (object)->callbacksreg = NULL;
#endif 
	klass = LVW_VISUI_CLASS (g_type_class_peek (LVW_VISUI_TYPE));
	parent_class = GTK_OBJECT_CLASS (g_type_class_peek_parent (klass));

	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(*GTK_OBJECT_CLASS (parent_class)->destroy) (object);

}

static void
lvw_visui_size_allocate (GtkWidget *widget,
		GtkAllocation *allocation)
{
	LvwVisUI *lvwuic = LVW_VISUI (widget);
	GtkAllocation child_allocation;

	widget->allocation = *allocation;

	if (GTK_WIDGET_REALIZED (widget))
		gdk_window_move_resize (lvwuic->event_window,
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
lvw_visui_size_request (GtkWidget *widget,
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
lvw_visui_realize (GtkWidget *widget)
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

	lvwuic->event_window = gdk_window_new (gtk_widget_get_parent_window (widget),
			&attributes, attributes_mask);
	gdk_window_set_user_data (lvwuic->event_window, lvwuic);

	widget->style = gtk_style_attach (widget->style, widget->window);
}

static void
lvw_visui_class_init (LvwVisUIClass *klass)
{
	GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->destroy = lvw_visui_destroy;
	widget_class->realize = lvw_visui_realize;
	widget_class->size_allocate = lvw_visui_size_allocate;
	widget_class->size_request = lvw_visui_size_request;
}

static void
lvw_visui_init (LvwVisUI *vuic)
{
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
lvw_visui_new (VisUIWidget *vuitree)
{
	GtkWidget *widget;
	LvwVisUI *vuic;

	vuic = g_object_new (lvw_visui_get_type (), NULL);
	vuic->callbacksreg = NULL;

	vuic->vuitree = vuitree;

	widget = lvw_visui_create_gtk_widgets (vuic, vuitree, 0);

	gtk_container_add (GTK_CONTAINER (vuic), widget);

	gtk_widget_show_all (GTK_WIDGET (vuic));

	return GTK_WIDGET (vuic);
}

/**
 * @}
 */

/*
 * Yes this function is big, a tad ugly, and it's farts just smell, very, very, badly.
 *
 * And it'll stay like this, this is one big monolithic, hey let's traverse that goddamn
 * VisUI tree and translate it to a super hot Gtk2 Widget function :)
 */
static GtkWidget *
lvw_visui_create_gtk_widgets (LvwVisUI *vuic, VisUIWidget *cont, int cnt)
{
	GtkWidget *widget;
	CallbackEntry *cbentry;

	char spacing[1024];
	VisUIWidgetType type = visual_ui_widget_get_type (cont);

	memset (spacing, '\t', cnt);
	spacing[cnt] = '\0';

	if (type == VISUAL_WIDGET_TYPE_BOX) {

		VisList *childs;
		VisListEntry *le = NULL;
		VisUIWidget *wi = NULL;

		printf ("%sBox entry: orientation %d\n", spacing, VISUAL_UI_BOX (cont)->orient);
		
		if (visual_ui_box_get_orient (VISUAL_UI_BOX (cont)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
			widget = gtk_hbox_new (FALSE, 10);
		else if (visual_ui_box_get_orient (VISUAL_UI_BOX (cont)) == VISUAL_ORIENT_TYPE_VERTICAL)
			widget = gtk_vbox_new (FALSE, 10);
		else
			return NULL;

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		childs = visual_ui_box_get_childs (VISUAL_UI_BOX (cont));

		while ((wi = visual_list_next (childs, &le)) != NULL) {
			GtkWidget *packer;
			
			packer = lvw_visui_create_gtk_widgets (vuic, wi, cnt + 1);

			gtk_box_pack_start (GTK_BOX (widget), packer, FALSE, FALSE, 0);
		}

		printf ("%sEnd box entry\n", spacing);

		return widget;
	} else if (type == VISUAL_WIDGET_TYPE_TABLE) {

		VisList *childs;
		VisListEntry *le = NULL;
		VisUITableEntry *tentry;

		printf ("%sTable entry: %d %d\n", spacing, VISUAL_UI_TABLE (cont)->rows, VISUAL_UI_TABLE (cont)->cols);

		widget = gtk_table_new (VISUAL_UI_TABLE (cont)->rows, VISUAL_UI_TABLE (cont)->cols, FALSE);

		gtk_table_set_row_spacings (GTK_TABLE(widget), 4);
		gtk_table_set_col_spacings (GTK_TABLE(widget), 4);

		childs = visual_ui_table_get_childs (VISUAL_UI_TABLE (cont));

		while ((tentry = visual_list_next (childs, &le)) != NULL) {
			GtkWidget *wi;

			wi = lvw_visui_create_gtk_widgets (vuic, tentry->widget, cnt + 1);

			gtk_table_attach_defaults (GTK_TABLE (widget), wi,
					tentry->col, tentry->col + 1, tentry->row, tentry->row + 1);

		}

		printf ("%sEnd table entry\n", spacing);

		return widget;
	
	} else if (type == VISUAL_WIDGET_TYPE_FRAME) {

		GtkWidget *child;

		printf ("%sFrame entry: name %s\n", spacing, VISUAL_UI_FRAME (cont)->name);

		widget = gtk_frame_new (VISUAL_UI_FRAME (cont)->name);
		
		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		if (VISUAL_UI_CONTAINER (cont)->child != NULL) {
			child = lvw_visui_create_gtk_widgets (vuic, VISUAL_UI_CONTAINER (cont)->child, cnt + 1);

			gtk_container_add (GTK_CONTAINER (widget), child);
		}
	
		printf ("%sEnd frame entry\n", spacing);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_LABEL) {

		GtkWidget *align;

		printf ("%sLabel entry: %s\n", spacing, visual_ui_label_get_text (VISUAL_UI_LABEL (cont)));

		if (VISUAL_UI_LABEL (cont)->bold == FALSE)
			widget = gtk_label_new (visual_ui_label_get_text (VISUAL_UI_LABEL (cont)));
		else {
			char *temp;
			int len;

			widget = gtk_label_new (NULL);
			
			len = strlen (visual_ui_label_get_text (VISUAL_UI_LABEL (cont))) + 8;
			temp = g_malloc (len);
			snprintf (temp, len, "<b>%s</b>", visual_ui_label_get_text (VISUAL_UI_LABEL (cont)));
			
			gtk_label_set_markup (GTK_LABEL (widget), temp);

			g_free (temp);
		}
					
		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		align = gtk_alignment_new (0, 0, 0, 0);
		gtk_container_add (GTK_CONTAINER (align), widget);
		
		return align;

	} else if (type == VISUAL_WIDGET_TYPE_IMAGE) {

		GdkPixbuf *pixbuf;
		GdkPixmap *pixmap;
		GdkDrawable *drawable;
		GdkColormap *colormap;
		GdkGC *gc;
		const VisVideo *video;
		unsigned char *buf;
		int x, y;
		int i = 0;

		printf ("%sImage entry\n", spacing);

		video = visual_ui_image_get_video (VISUAL_UI_IMAGE (cont));

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


		buf = video->pixels;
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

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_SEPARATOR) {

		printf ("%sSeparator entry\n", spacing);

		if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (cont)) == VISUAL_ORIENT_TYPE_NONE) {
			visual_log (VISUAL_LOG_CRITICAL, "Separator orientation must be HORIZONTAL or VERTICAL");

			return NULL;
		}

		if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (cont)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
			widget = gtk_hseparator_new ();
		else
			widget = gtk_vseparator_new ();

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_ENTRY) {

		VisParamEntry *param;

		printf ("%sText entry\n", spacing);

		widget = gtk_entry_new ();

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_entry, cont);

		gtk_entry_set_text (GTK_ENTRY (widget), visual_param_entry_get_string (param));
		gtk_entry_set_max_length (GTK_ENTRY (widget), VISUAL_UI_ENTRY (cont)->length);

		g_signal_connect (G_OBJECT (widget), "changed",
				G_CALLBACK (cb_visui_entry), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_entry;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_SLIDER) {

		VisParamEntry *param;
		double val;

		printf ("%sSlider entry\n", spacing);

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		if (param->type == VISUAL_PARAM_TYPE_INTEGER)
			val = visual_param_entry_get_integer (param);
		else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
			val = visual_param_entry_get_float (param);
		else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
			val = visual_param_entry_get_double (param);
		else {
			visual_log (VISUAL_LOG_CRITICAL, "Param for numeric widget must be of numeric type");

			return NULL;
		}

		widget = gtk_hscale_new_with_range (VISUAL_UI_RANGE (cont)->min,
				VISUAL_UI_RANGE (cont)->max,
				VISUAL_UI_RANGE (cont)->step);

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_slider, cont);

		if (VISUAL_UI_SLIDER (cont)->showvalue == FALSE)
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		else
			gtk_scale_set_draw_value (GTK_SCALE (widget), TRUE);

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		gtk_scale_set_digits (GTK_SCALE (widget), VISUAL_UI_RANGE (cont)->precision);
		
		gtk_range_set_value (GTK_RANGE (widget), val); 

		g_signal_connect (G_OBJECT (widget), "value-changed",
				G_CALLBACK (cb_visui_slider), cont);
		
		return widget;
		
	} else if (type == VISUAL_WIDGET_TYPE_NUMERIC) {

		VisParamEntry *param;
		double val;

		printf ("%sNumeric entry\n", spacing);

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		if (param->type == VISUAL_PARAM_TYPE_INTEGER)
			val = visual_param_entry_get_integer (param);
		else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
			val = visual_param_entry_get_float (param);
		else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
			val = visual_param_entry_get_double (param);
		else {
			visual_log (VISUAL_LOG_CRITICAL, "Param for numeric widget must be of numeric type");

			return NULL;
		}

		widget = gtk_spin_button_new_with_range (VISUAL_UI_RANGE (cont)->min,
				VISUAL_UI_RANGE (cont)->max,
				VISUAL_UI_RANGE (cont)->step);

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_numeric, cont);
		
		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
	
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (widget), TRUE);
		gtk_spin_button_set_digits (GTK_SPIN_BUTTON (widget), VISUAL_UI_RANGE (cont)->precision);
		
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), val); 

		g_signal_connect (G_OBJECT (widget), "changed",
				G_CALLBACK (cb_visui_numeric), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_numeric;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_COLOR) {

		VisParamEntry *param;
		VisColor *color;
		GdkColor gdkcol;

		printf ("%sColor entry\n", spacing);

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		if (param->type != VISUAL_PARAM_TYPE_COLOR) {
			visual_log (VISUAL_LOG_CRITICAL, "Param for color widget must be of color type");

			return NULL;
		}

		color = visual_param_entry_get_color (param);
		
		gdkcol.red = color->r * (65535 / 255);
		gdkcol.blue = color->b * (65535 / 255);
		gdkcol.green = color->g * (65535 / 255);

		widget = gtk_color_selection_new ();

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_color, cont);

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (widget), &gdkcol);

		g_signal_connect (G_OBJECT (widget), "color-changed",
				G_CALLBACK (cb_visui_color), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_color;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_POPUP) {

		VisParamEntry *param;
		VisUIChoiceList *options;
		VisUIChoiceEntry *centry;
		VisListEntry *le = NULL;
		VisList *choices;

		printf ("%sPopup entry\n", spacing);
		
		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (cont));
		choices = &options->choices;

		if (options->type != VISUAL_CHOICE_TYPE_SINGLE) {
			visual_log (VISUAL_LOG_CRITICAL, "Selection type for popup widget must be of type single");

			return NULL;
		}

		widget = gtk_combo_box_new_text ();


		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_popup, cont);

		gtk_widget_set_size_request (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		while ((centry = visual_list_next (choices, &le)) != NULL)
			gtk_combo_box_append_text (GTK_COMBO_BOX (widget), centry->name);

		gtk_combo_box_set_active (GTK_COMBO_BOX (widget), visual_ui_choice_get_active (VISUAL_UI_CHOICE (cont)));

		g_signal_connect (G_OBJECT (widget), "changed",
				G_CALLBACK (cb_visui_popup), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_popup;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_LIST) {

		printf ("%sList entry\n", spacing);

		/* FIXME implement this */

	} else if (type == VISUAL_WIDGET_TYPE_RADIO) {
		
		GtkWidget *radio;
		VisParamEntry *param;
		VisUIChoiceList *options;
		VisUIChoiceEntry *centry;
		VisListEntry *le = NULL;
		VisList *choices;
		
		printf ("%sRadio entry\n", spacing);

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));
		
		options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (cont));
		choices = &options->choices;

		if (options->type != VISUAL_CHOICE_TYPE_SINGLE) {
			visual_log (VISUAL_LOG_CRITICAL, "Selection type for radio button widget must be of type single");

			return NULL;
		}

		if (options->count < 2) {
			visual_log (VISUAL_LOG_CRITICAL, "The number of choices for a radio button must be atleast 2");

			return NULL;
		}

		if (VISUAL_UI_RADIO (cont)->orient == VISUAL_ORIENT_TYPE_HORIZONTAL)
			widget = gtk_hbox_new (FALSE, 10);
		else
			widget = gtk_vbox_new (FALSE, 10);
		
		/* First entry */
		centry = visual_list_next (choices, &le);
		radio = gtk_radio_button_new_with_label (NULL, centry->name);
		
		gtk_widget_set_size_request (GTK_WIDGET (radio),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		g_signal_connect (G_OBJECT (radio), "toggled",
				G_CALLBACK (cb_visui_radio), cont);

		gtk_box_pack_start (GTK_BOX (widget), radio, FALSE, FALSE, 0);
		
		while ((centry = visual_list_next (choices, &le)) != NULL) {
			radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio), centry->name);
			
			gtk_widget_set_size_request (GTK_WIDGET (radio),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
	
			g_signal_connect (G_OBJECT (radio), "toggled",
				G_CALLBACK (cb_visui_radio), cont);

			gtk_box_pack_start (GTK_BOX (widget), radio, FALSE, FALSE, 0);
		}

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_radio, cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_radio;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_CHECKBOX) {

		VisParamEntry *param;
		VisUIChoiceList *options;

		printf ("%sCheckbox entry\n", spacing);

		param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (cont));

		if (options->type != VISUAL_CHOICE_TYPE_SINGLE) {
			visual_log (VISUAL_LOG_CRITICAL, "Selection type for checkbox widget must be of type single");

			return NULL;
		}

		if (options->count != 2) {
			visual_log (VISUAL_LOG_CRITICAL, "The number of choices for a checkbox must be 2");

			return NULL;
		}

		if (VISUAL_UI_CHECKBOX (cont)->name != NULL)
			widget = gtk_check_button_new_with_label (VISUAL_UI_CHECKBOX (cont)->name);
		else
			widget = gtk_check_button_new ();

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), visual_ui_choice_get_active (VISUAL_UI_CHOICE (cont)));

		visual_ui_widget_set_private (cont, widget);
		visual_param_entry_add_callback (param, cb_param_checkbox, cont);

		g_signal_connect (G_OBJECT (widget), "toggled",
				G_CALLBACK (cb_visui_checkbox), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->callback = cb_param_checkbox;
		vuic->callbacksreg = g_slist_append (vuic->callbacksreg, cbentry);

		return widget;
	}

	printf ("%sUnhandled type: %d\n", spacing, type);

	return NULL;
}

/* Parameter change callbacks from within GTK */
static void
cb_visui_entry (GtkEditable *editable, gpointer user_data)
{
	VisParamEntry *param;
	const char *text;

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

	if (param->type == VISUAL_PARAM_TYPE_INTEGER)
		visual_param_entry_set_integer (param, value);
	else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
		visual_param_entry_set_float (param, value);
	else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
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

	if (param->type == VISUAL_PARAM_TYPE_INTEGER)
		visual_param_entry_set_integer (param, value);
	else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
		visual_param_entry_set_float (param, value);
	else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
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

	if (param->type != VISUAL_PARAM_TYPE_COLOR)
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the color selector isn't a color param");

	visual_param_entry_set_color (param, color.red >> 8, color.green >> 8, color.blue >> 8);
}

static void
cb_visui_popup (GtkComboBox *combobox, gpointer user_data)
{
	int active;
	/* FIXME remove reference to param, is for debug purpose */
	const VisParamEntry *param;

	active = gtk_combo_box_get_active (combobox);

	visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), active);

	param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));
	
	printf ("Popup changed, active: %d\n", active);
}

static void
cb_visui_radio (GtkToggleButton *togglebutton, gpointer user_data)
{
	GSList *group, *temp;
	GtkRadioButton *but;
	int entries = 0;
	int cnt = 0;

	const VisParamEntry *param;

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
	const VisParamEntry *param;

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
cb_param_entry (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_entry_set_text (GTK_ENTRY (visual_ui_widget_get_private (widget)),
				visual_param_entry_get_string (param));

	g_free (data);

	return FALSE;
}

static void
cb_param_slider (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	double val = 0;

	if (param->type == VISUAL_PARAM_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);

	gtk_range_set_value (GTK_RANGE (visual_ui_widget_get_private (widget)),
			val); 

	g_free (data);

	return FALSE;
}

static void
cb_param_numeric (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	GtkAdjustment *adj;
	double val = 0;

	if (param->type == VISUAL_PARAM_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);

	adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (visual_ui_widget_get_private (widget)));
	gtk_adjustment_set_value (adj, val);

	g_free (data);

	return FALSE;
}

static void
cb_param_color (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	VisColor *color;
	GdkColor gdkcol;

	color = visual_param_entry_get_color (param);
	
	/* FIXME on 255, 254 is displayed.. why ? */
	gdkcol.red = color->r * (65535 / 255);
	gdkcol.blue = color->b * (65535 / 255);
	gdkcol.green = color->g * (65535 / 255);

	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (visual_ui_widget_get_private (widget)),
			&gdkcol);

	g_free (data);

	return FALSE;
}

static void
cb_param_popup (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_combo_box_set_active (GTK_COMBO_BOX (visual_ui_widget_get_private (widget)),
			visual_ui_choice_get_active (widget));

	g_free (data);

	return FALSE;
}
	
static void
cb_param_radio (const VisParamEntry *param, void *priv)
{
	printf ("Param RADIO callback\n");
}

static gboolean
cb_idle_radio (void *userdata)
{
	PrivIdleData *data = userdata;
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	g_free (data);

	return FALSE;
}

static void
cb_param_checkbox (const VisParamEntry *param, void *priv)
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
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (visual_ui_widget_get_private (widget)),
			visual_ui_choice_get_active (widget));

	g_free (data);

	return FALSE;
}

