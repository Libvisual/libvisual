#ifndef _LVW_GTK2_VISUI_H
#define _LVW_GTK2_VISUI_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <libvisual/libvisual.h>

G_BEGIN_DECLS

#define LVW_UICONTAINER_TYPE            (lvwidget_uicontainer_get_type ())
#define LVW_UICONTAINER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LVW_UICONTAINER_TYPE, LvwUIContainer))
#define LVW_UICONTAINER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LVW_UICONTAINER_TYPE, LvwUIContainerClass))
#define IS_LVW_UICONTAINER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LVW_UICONTAINER_TYPE))
#define IS_LVW_UICONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LVW_UICONTAINER_TYPE))


typedef struct _LvwUIContainer       LvwUIContainer;
typedef struct _LvwUIContainerClass  LvwUIContainerClass;

struct _LvwUIContainer
{
	GtkBin parent;

	VisUIWidget *vuitree;
	GSList *callbacksreg;
	GdkWindow *event_window;
};

struct _LvwUIContainerClass
{
	GtkBinClass parent_class;

	void (* lvwuicontainer) (LvwUIContainer *vuic);
};

GType          lvwidget_uicontainer_get_type        (void);
GtkWidget*     lvwidget_uicontainer_new             (VisUIWidget *vuitree);

G_END_DECLS

#endif /* _LVW_GTK2_VISUI_H */
