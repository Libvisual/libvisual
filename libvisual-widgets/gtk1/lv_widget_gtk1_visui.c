
#include "lv_widget_gtk1_visui.h"


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
	const VisParamEntry	*param;
	void			*priv;
};

struct _CallbackEntry {
	const VisParamEntry	*param;
	int			 id;
};


static GtkWidgetClass *parent_class = NULL;


static void
lvw_visui_realize (GtkWidget *widget)
{
	LvwVisUI *lvwuic;
	GdkWindowAttr attributes;
	gint attributes_mask;
	gint border_width;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_LVW_VISUI(widget));

	lvwuic = LVW_VISUI (widget);

	g_return_if_fail (lvwuic != NULL);

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
	gdk_window_ref (widget->window);

	lvwuic->priv->event_window = gdk_window_new (gtk_widget_get_parent_window (widget),
			&attributes, attributes_mask);
	gdk_window_set_user_data (lvwuic->priv->event_window, lvwuic);

	widget->style = gtk_style_attach (widget->style, widget->window);
		
	lvwuic->priv->tooltips = gtk_tooltips_new ();
}

static void
lvw_visui_size_allocate (GtkWidget *widget,
		GtkAllocation *allocation)
{
	LvwVisUI *lvwuic;
	GtkAllocation child_allocation;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_LVW_VISUI(widget));

	lvwuic = LVW_VISUI (widget);

	g_return_if_fail (lvwuic != NULL);

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
lvw_visui_size_request (GtkWidget *widget,
		GtkRequisition *requisition)
{ 
	LvwVisUI *lvwuic;
	gint focus_width; 
	gint focus_pad; 

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_LVW_VISUI(widget));

	lvwuic = LVW_VISUI (widget);

	g_return_if_fail (lvwuic != NULL);

	requisition->width = (GTK_CONTAINER (widget)->border_width);
	requisition->height = (GTK_CONTAINER (widget)->border_width);
	
	if (GTK_BIN (lvwuic)->child && GTK_WIDGET_VISIBLE (GTK_BIN (lvwuic)->child)) {
		GtkRequisition child_requisition;

		gtk_widget_size_request (GTK_BIN (lvwuic)->child, &child_requisition);

		requisition->width += child_requisition.width;
		requisition->height += child_requisition.height;
	}
}
static void lvw_class_init (LvwVisUIClass *klass)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;

	object_class = (GtkObjectClass*) klass;
	widget_class = (GtkWidgetClass*) klass;
	container_class = (GtkContainerClass*) klass;

	parent_class = gtk_type_class (gtk_widget_get_type ());

	widget_class->realize = lvw_visui_realize;
	widget_class->size_request = lvw_visui_size_request;
	widget_class->size_allocate = lvw_visui_size_allocate;
}

static void lvw_init (LvwVisUI *vuic)
{

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
cb_visui_slider (GtkAdjustment *adj, gpointer user_data)
{
	VisUIWidget *container;
	VisParamEntry *param;
	gdouble value;

	container = user_data;
	g_return_if_fail (container != NULL);

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (container));

	value = GTK_ADJUSTMENT(adj)->value;

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
cb_visui_popup (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
	GtkRadioMenuItem *radiomenuitem = GTK_RADIO_MENU_ITEM (checkmenuitem);
	int active;
	GSList *group, *l;

	/* Catch off the unactivate toggle */
	/*if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (checkmenuitem)) == FALSE)
		return;*/

	group = gtk_radio_menu_item_group (radiomenuitem);

	active = g_slist_length (group);
	
	l = group;
	
	/*while (l) {
		if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (l->data)))
			break;

		active--;
		l = g_slist_next (l);
	}*/

	/* The choice group indexing starts at 0, not 1 */
	active--;

	if (active < 0)
		active = 0;
	
	visual_ui_choice_set_active (VISUAL_UI_CHOICE (user_data), active);

	printf ("Popup changed, toggle: %d\n", active);
}

static void
cb_visui_color (GtkColorSelection *colorselection, gpointer user_data)
{
	VisParamEntry *param;
	gdouble color[4];

	gtk_color_selection_get_color (colorselection, color);

	param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (user_data));

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR)
		visual_log (VISUAL_LOG_CRITICAL, "The param connected to the color selector isn't a color param");

	visual_param_entry_set_color (param, color[0] * 255, color[1] * 255, color[2] * 255);

	printf ("Color changed: %lf %lf %lf\n", color[0], color[1], color[2]);
}

/* Parameter change callbacks from within VisParam */
static gboolean
cb_idle_entry (void *userdata)
{
	PrivIdleData *data = userdata;
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

	gtk_entry_set_text (GTK_ENTRY (visual_object_get_private (VISUAL_OBJECT (widget))),
				visual_param_entry_get_string (param));

	g_free (data);

	return FALSE;
}

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
cb_idle_slider (void *userdata)
{
	GtkAdjustment *adj;
	PrivIdleData *data = userdata;
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	double val = 0;

	if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		val = visual_param_entry_get_integer (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		val = visual_param_entry_get_float (param);
	else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		val = visual_param_entry_get_double (param);

	adj = gtk_range_get_adjustment (GTK_RANGE (visual_object_get_private (VISUAL_OBJECT (widget)))); 
	gtk_adjustment_set_value (adj, val);

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
cb_idle_popup (void *userdata)
{
	PrivIdleData *data = userdata;
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;

#if HAVE_GTK_AT_LEAST_2_4_X
	gtk_combo_box_set_active (GTK_COMBO_BOX (visual_object_get_private (VISUAL_OBJECT (widget))),
			visual_ui_choice_get_active (widget));
#endif

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
cb_idle_color (void *userdata)
{
	PrivIdleData *data = userdata;
	const VisParamEntry *param = data->param;
	VisUIWidget *widget = data->priv;
	VisColor *color;
	gdouble dcolor[4];

	color = visual_param_entry_get_color (param);
	
	dcolor[0] = color->r / 65535.0;
	dcolor[1] = color->g / 65535.0;
	dcolor[2] = color->b / 65535.0;

	gtk_color_selection_set_color (GTK_COLOR_SELECTION (visual_object_get_private (VISUAL_OBJECT (widget))),
			dcolor);
				
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

/*
 * Yes this function is big, a tad ugly, and it's farts just smell, very, very, badly.
 *
 * And it'll stay like this, this is one big monolithic, hey let's traverse that goddamn
 * VisUI tree and translate it to a super hot Gtk2 Widget function :)
 */
static GtkWidget *
lvw_visui_create_gtk_widgets (LvwVisUI *vuic, VisUIWidget *cont)
{
	const char *tooltip = NULL;
	GtkWidget *widget;
	CallbackEntry *cbentry;
	VisUIWidgetType type;

	g_return_val_if_fail (vuic != NULL, NULL);
	g_return_val_if_fail (vuic->priv != NULL, NULL);
	g_return_val_if_fail (cont != NULL, NULL);

	type = visual_ui_widget_get_type (cont);

	if (type == VISUAL_WIDGET_TYPE_BOX) {

		VisList *childs;
		VisListEntry *le = NULL;
		VisUIWidget *wi = NULL;

		if (visual_ui_box_get_orient (VISUAL_UI_BOX (cont)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
			widget = gtk_hbox_new (FALSE, 10);
		else if (visual_ui_box_get_orient (VISUAL_UI_BOX (cont)) == VISUAL_ORIENT_TYPE_VERTICAL)
			widget = gtk_vbox_new (FALSE, 10);
		else
			return NULL;

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		childs = visual_ui_box_get_childs (VISUAL_UI_BOX (cont));

		while ((wi = visual_list_next (childs, &le)) != NULL) {
			GtkWidget *packer;
			
			packer = lvw_visui_create_gtk_widgets (vuic, wi);

			gtk_box_pack_start (GTK_BOX (widget), packer, FALSE, FALSE, 0);
		}
		
		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_TABLE) {

		VisList *childs;
		VisListEntry *le = NULL;
		VisUITableEntry *tentry;

		widget = gtk_table_new (VISUAL_UI_TABLE (cont)->rows, VISUAL_UI_TABLE (cont)->cols, FALSE);

		gtk_table_set_row_spacings (GTK_TABLE(widget), 4);
		gtk_table_set_col_spacings (GTK_TABLE(widget), 4);

		childs = visual_ui_table_get_childs (VISUAL_UI_TABLE (cont));

		while ((tentry = visual_list_next (childs, &le)) != NULL) {
			GtkWidget *wi;

			wi = lvw_visui_create_gtk_widgets (vuic, tentry->widget);

			gtk_table_attach_defaults (GTK_TABLE (widget), wi,
					tentry->col, tentry->col + 1, tentry->row, tentry->row + 1);

		}

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip); 

		return widget;
	
	} else if (type == VISUAL_WIDGET_TYPE_FRAME) {

		GtkWidget *child;

		widget = gtk_frame_new (VISUAL_UI_FRAME (cont)->name);
		
		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		if (VISUAL_UI_CONTAINER (cont)->child != NULL) {
			child = lvw_visui_create_gtk_widgets (vuic, VISUAL_UI_CONTAINER (cont)->child);

			gtk_container_add (GTK_CONTAINER (widget), child);
		}

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip); 

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_LABEL) {

		GtkWidget *align;

		/*
		 * TODO manage bold face fonts
		 * 
		 * if (VISUAL_UI_LABEL (cont)->bold == FALSE)*/
			widget = gtk_label_new (visual_ui_label_get_text (VISUAL_UI_LABEL (cont)));
		/*else {
			char *temp;
			int len;

			widget = gtk_label_new (NULL);
			
			len = strlen (visual_ui_label_get_text (VISUAL_UI_LABEL (cont))) + 8;
			temp = g_malloc (len);
			snprintf (temp, len, "<b>%s</b>", visual_ui_label_get_text (VISUAL_UI_LABEL (cont)));
			
			gtk_label_set_markup (GTK_LABEL (widget), temp);

			g_free (temp);
		}*/
					
		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		align = gtk_alignment_new (0, 0, 0, 0);
		gtk_container_add (GTK_CONTAINER (align), widget);
	
		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip); 

		return align;

	} else if (type == VISUAL_WIDGET_TYPE_IMAGE) {
		/* TODO */
		return NULL;

	} else if (type == VISUAL_WIDGET_TYPE_SEPARATOR) {

		if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (cont)) == VISUAL_ORIENT_TYPE_NONE) {
			visual_log (VISUAL_LOG_CRITICAL, "Separator orientation must be HORIZONTAL or VERTICAL");

			return NULL;
		}

		if (visual_ui_separator_get_orient (VISUAL_UI_SEPARATOR (cont)) == VISUAL_ORIENT_TYPE_HORIZONTAL)
			widget = gtk_hseparator_new ();
		else
			widget = gtk_vseparator_new ();

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_ENTRY) {

		VisParamEntry *param;

		widget = gtk_entry_new ();

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		visual_object_set_private (VISUAL_OBJECT (cont), widget);
		
		gtk_entry_set_text (GTK_ENTRY (widget), visual_param_entry_get_string (param));
		gtk_entry_set_max_length (GTK_ENTRY (widget), VISUAL_UI_ENTRY (cont)->length);

		gtk_signal_connect (GTK_OBJECT (widget), "changed",
				GTK_SIGNAL_FUNC (cb_visui_entry), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_entry, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_SLIDER) {

		GtkAdjustment *adj;		
		const VisParamEntry *param;
		double val;

		param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

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

		adj = gtk_adjustment_new (val,
				VISUAL_UI_RANGE(cont)->min,
				VISUAL_UI_RANGE(cont)->max,
				VISUAL_UI_RANGE(cont)->step,
				VISUAL_UI_RANGE(cont)->step,
				VISUAL_UI_RANGE(cont)->step);

		widget = gtk_hscale_new (adj);

		gtk_scale_set_digits (GTK_SCALE (widget), VISUAL_UI_RANGE (cont)->precision);

		if (VISUAL_UI_SLIDER (cont)->showvalue == FALSE)
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		else {
			gtk_scale_set_draw_value (GTK_SCALE (widget), TRUE);
			gtk_scale_set_value_pos (GTK_SCALE (widget), GTK_POS_RIGHT);
			gtk_scale_draw_value (GTK_SCALE (widget));
		}

		visual_object_set_private (VISUAL_OBJECT (cont), widget);

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
		
		gtk_signal_connect (GTK_OBJECT (adj), "value-changed",
				GTK_SIGNAL_FUNC (cb_visui_slider), cont);
		
		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_slider, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_NUMERIC) {

		GtkAdjustment *adj;
		VisParamEntry *param;
		double val;

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		if (param->type == VISUAL_PARAM_ENTRY_TYPE_INTEGER)
			val = (double)visual_param_entry_get_integer (param);
		else if (param->type == VISUAL_PARAM_ENTRY_TYPE_FLOAT)
			val = (double)visual_param_entry_get_float (param);
		else if (param->type == VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
			val = visual_param_entry_get_double (param);
		else {
			visual_log (VISUAL_LOG_CRITICAL, "Param for numeric widget must be of numeric type");

			return NULL;
		}

		adj = gtk_adjustment_new (val,
				VISUAL_UI_RANGE(cont)->min,
				VISUAL_UI_RANGE(cont)->max,
				VISUAL_UI_RANGE(cont)->step,
				VISUAL_UI_RANGE(cont)->step,
				VISUAL_UI_RANGE(cont)->step);

		widget = gtk_spin_button_new (adj, VISUAL_UI_RANGE (cont)->step, VISUAL_UI_RANGE (cont)->precision);

		gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON(widget), adj);

		visual_object_set_private (VISUAL_OBJECT (cont), widget);

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
	
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (widget), TRUE);

		/*gtk_signal_connect (GTK_OBJECT (widget), "changed",
				GTK_SIGNAL_FUNC (cb_visui_numeric), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_numeric, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);*/

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_COLOR) {

		VisParamEntry *param;
		VisColor *color;
		gdouble dcolor[4];

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
			visual_log (VISUAL_LOG_CRITICAL, "Param for color widget must be of color type");

			return NULL;
		}

		color = visual_param_entry_get_color (param);
		
		dcolor[0] = color->r / 65535.0;
		dcolor[1] = color->g / 65535.0;
		dcolor[2] = color->b / 65535.0;

		widget = gtk_color_selection_new ();

		visual_object_set_private (VISUAL_OBJECT (cont), widget);

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);
	
		gtk_color_selection_set_opacity (GTK_COLOR_SELECTION (widget), FALSE);

		gtk_color_selection_set_color (GTK_COLOR_SELECTION (widget), dcolor);

		gtk_signal_connect (GTK_OBJECT (widget), "color_changed",
				GTK_SIGNAL_FUNC (cb_visui_color), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_color, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip); 

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_POPUP) {

		VisParamEntry *param;
		VisUIChoiceList *options;
		VisUIChoiceEntry *centry;
		VisListEntry *le = NULL;
		VisList *choices;
		GtkWidget *menu, *menuitem;
		GSList *group;

		param = (VisParamEntry*)visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (cont));
		choices = &options->choices;

		/*if (options->type != VISUAL_CHOICE_TYPE_SINGLE) {
			visual_log (VISUAL_LOG_CRITICAL, "Selection type for popup widget must be of type single");

			return NULL;
		}*/

		widget = gtk_option_menu_new ();

		gtk_widget_set_usize (GTK_WIDGET (widget),
				VISUAL_UI_WIDGET (cont)->width,
				VISUAL_UI_WIDGET (cont)->height);

		menu = gtk_menu_new ();
		group = NULL;

		while ((centry = visual_list_next (choices, &le)) != NULL) {
			menuitem = gtk_radio_menu_item_new_with_label (group, centry->name);
			group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM(menuitem));
			gtk_menu_append (GTK_MENU(menu), menuitem);
			gtk_widget_show (menuitem);

			gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
					GTK_SIGNAL_FUNC (cb_visui_popup), cont);
		}

		gtk_option_menu_set_menu (GTK_OPTION_MENU(widget), menu);

		visual_object_set_private (VISUAL_OBJECT (cont), widget);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_popup, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip); 

		return widget;

	} else if (type == VISUAL_WIDGET_TYPE_CHECKBOX) {

		VisParamEntry *param;
		VisUIChoiceList *options;

		param = visual_ui_mutator_get_param (VISUAL_UI_MUTATOR (cont));

		options = visual_ui_choice_get_choices (VISUAL_UI_CHOICE (cont));

		/*if (options->type != VISUAL_CHOICE_TYPE_SINGLE) {
			visual_log (VISUAL_LOG_CRITICAL, "Selection type for checkbox widget must be of type single");

			return NULL;
		}*/

		if (options->count != 2) {
			visual_log (VISUAL_LOG_CRITICAL, "The number of choices for a checkbox must be 2");

			return NULL;
		}

		if (VISUAL_UI_CHECKBOX (cont)->name != NULL)
			widget = gtk_check_button_new_with_label (VISUAL_UI_CHECKBOX (cont)->name);
		else
			widget = gtk_check_button_new ();

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), visual_ui_choice_get_active (VISUAL_UI_CHOICE (cont)));

		visual_object_set_private (VISUAL_OBJECT (cont), widget);

		/*gtk_signal_connect (GTK_OBJECT (widget), "toggled",
				GTK_SIGNAL_FUNC (cb_visui_checkbox), cont);

		cbentry = g_new0 (CallbackEntry, 1);
		cbentry->param = param;
		cbentry->id = visual_param_entry_add_callback (param, cb_param_checkbox, cont);
		vuic->priv->callbacksreg = g_slist_append (vuic->priv->callbacksreg, cbentry);*/

		tooltip = visual_ui_widget_get_tooltip (cont);
		if (tooltip != NULL)
			gtk_tooltips_set_tip (GTK_TOOLTIPS (vuic->priv->tooltips), widget, tooltip, tooltip);

		return widget;
	}

	return NULL;
}

GtkType lvw_visui_get_type (void)
{
	static GtkType lvw_type = 0;

	if (!lvw_type) {
		GtkTypeInfo lvw_info = {
			"LvwVisUI",
			sizeof (LvwVisUI),
			sizeof (LvwVisUIClass),
			(GtkClassInitFunc) lvw_class_init,
			(GtkObjectInitFunc) lvw_init,
			/*(GtkArgSetFunc) lvw_set_arg,*/
			(GtkArgSetFunc) NULL,
			(GtkArgGetFunc) NULL
		};

		lvw_type = gtk_type_unique (gtk_bin_get_type (), &lvw_info);
	}

	return lvw_type;
}

/**
 * @defgroup LvwVisUI LvwVisUI
 * @{
 */

/**
 * Creates a GtkWidget from a VisUIWidget. This is essentially the GTK drive for VisUIWidget, that means
 * that you can generate native config dialogs using this function.
 *
 * @param vuitree Pointer to the VisUIWidget that contains the user interface that needs to be converted to a
 * 	native GTK version.
 *
 * @return A GtkWidget containing the in the VisUIWidget described user interface.
 */
GtkWidget*
lvw_visui_new (VisUIWidget *vuitree)
{
	GtkWidget *widget;
	LvwVisUI *vuic;

	g_return_val_if_fail (vuitree != NULL, NULL);

	/* Ref it so it won't disappear under our feets */
	visual_object_ref (VISUAL_OBJECT (vuitree));

	vuic = gtk_type_new (lvw_visui_get_type ());

	/* FIXME should move into the object creation! */
	vuic->priv = g_new0 (LvwVisUIPrivate, 1);

	vuic->priv->callbacksreg = NULL;

	vuic->priv->vuitree = vuitree;

	widget = lvw_visui_create_gtk_widgets (vuic, vuitree);

	gtk_container_add (GTK_CONTAINER (vuic), widget);

	gtk_widget_show_all (GTK_WIDGET (vuic));

	return GTK_WIDGET (vuic);
}

/**
 * @}
 */

