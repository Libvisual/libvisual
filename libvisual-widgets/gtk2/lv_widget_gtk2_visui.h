/* Libvisual-widgets - The standard widgets for libvisual.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Milosz Derezynski <md@relevantive.de>
 *
 * $Id:
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

#ifndef _LVW_GTK2_VISUI_H
#define _LVW_GTK2_VISUI_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <libvisual/libvisual.h>

G_BEGIN_DECLS

#define LVW_VISUI_TYPE            (lvwidget_visui_get_type ())
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

	void (* lvwidgetuicontainer) (LvwVisUI *vuic);
};

GType          lvwidget_visui_get_type        (void);
GtkWidget*     lvwidget_visui_new             (VisUIWidget *vuitree);

G_END_DECLS

#endif /* _LVW_GTK2_VISUI_H */
