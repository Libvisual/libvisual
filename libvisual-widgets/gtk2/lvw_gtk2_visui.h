#ifndef _LVW_GTK2_VISUI_H
#define _LVW_GTK2_VISUI_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <libvisual/libvisual.h>

G_BEGIN_DECLS

#define LVW_VISUI_TYPE            (lvw_visui_get_type ())
#define LVW_VISUI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LVW_VISUI_TYPE, LvwVisUI))
#define LVW_VISUI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LVW_VISUI_TYPE, LvwVisUIClass))
#define IS_LVW_VISUI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LVW_VISUI_TYPE))
#define IS_LVW_VISUI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LVW_VISUI_TYPE))


typedef struct _LvwVisUI	LvwVisUI;
typedef struct _LvwVisUIClass	LvwVisUIClass;
typedef struct _LvwVisUIPrivate	LvwVisUIPrivate;

struct _LvwVisUI
{
	GtkBin parent;
	
	LvwVisUIPrivate *priv;
};

struct _LvwVisUIClass
{
	GtkBinClass parent_class;

	void (* lvwuicontainer) (LvwVisUI *vuic);
};

GType          lvw_visui_get_type        (void);
GtkWidget*     lvw_visui_new             (VisUIWidget *vuitree);

G_END_DECLS

#endif /* _LVW_GTK2_VISUI_H */
