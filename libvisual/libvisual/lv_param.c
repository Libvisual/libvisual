/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_param.c,v 1.50 2006/01/22 13:23:37 synap Exp $
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

#include "config.h"
#include "lv_param.h"
#include "lv_common.h"
#include "lv_util.h"
#include <string.h>

static void visual_param_list_dtor (VisObject *object);
static void visual_param_dtor      (VisObject *object);

VisParamList *visual_param_list_new (void)
{
    VisParamList *self = visual_mem_new0 (VisParamList, 1);

    visual_object_init (VISUAL_OBJECT (self), visual_param_list_dtor);

    self->entries = visual_list_new (visual_object_collection_destroyer);

    return self;
}

void visual_param_list_dtor (VisObject *obj)
{
    VisParamList *self = VISUAL_PARAM_LIST (obj);
    visual_object_unref (VISUAL_OBJECT (self->entries));
}

void visual_param_list_set_eventqueue (VisParamList *self, VisEventQueue *eventqueue)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (eventqueue != NULL);

    self->eventqueue = eventqueue;
}

VisEventQueue *visual_param_list_get_eventqueue (VisParamList *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->eventqueue;
}

int visual_param_list_add (VisParamList *self, VisParam *param)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (param != NULL, FALSE);

    if (visual_list_add (self->entries, param)) {
        param->parent = self;
        return TRUE;
    }

    return FALSE;
}

unsigned int visual_param_list_add_many (VisParamList *self, VisParam **params, unsigned int nparams)
{
    visual_return_val_if_fail (self   != NULL, 0);
    visual_return_val_if_fail (params != NULL, 0);

    unsigned int i;
    unsigned int added = 0;

    for (i = 0; i < nparams; i++) {
        if (visual_param_list_add (self, params[i])) {
            added++;
        }
    }

    return added;
}

int visual_param_list_remove (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (name != NULL, FALSE);

    VisListEntry *le = NULL;
    VisParam *param;

    while ((param = visual_list_next (self->entries, &le)) != NULL) {

        if (strcmp (param->name, name) == 0) {
            visual_list_delete (self->entries, &le);

            return TRUE;
        }
    }

    return FALSE;
}

VisParam *visual_param_list_get (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (name != NULL, NULL);

    VisListEntry *le = NULL;
    VisParam *param;

    while ((param = visual_list_next (self->entries, &le)) != NULL) {
        param = le->data;

        if (strcmp (param->name, name) == 0)
            return param;
    }

    return NULL;
}

VisParam *visual_param_new (const char * name,
                            const char * description,
                            VisParamType type,
                            void *       default_value)
{
    visual_return_val_if_fail (name != NULL, NULL);
    visual_return_val_if_fail (type != VISUAL_PARAM_TYPE_NONE, NULL);
    visual_return_val_if_fail (default_value != NULL, NULL);

    VisParam *self = visual_mem_new0 (VisParam, 1);

    visual_object_init (VISUAL_OBJECT (self), visual_param_dtor);

    self->name        = visual_strdup (name);
    self->description = visual_strdup (description);

    visual_param_value_set (&self->value, default_value);
    visual_param_value_set (&self->default_value, default_value);

    self->changed_handlers = visual_list_new (visual_object_collection_destroyer);

    return self;
}

void visual_param_dtor (VisObject *obj)
{
    VisParam *self = VISUAL_PARAM (obj);

    visual_mem_free (self->name);
    visual_mem_free (self->description);

    visual_param_value_free_value (&self->value);
    visual_param_value_free_value (&self->default_value);

    visual_object_unref (VISUAL_OBJECT (self->changed_handlers));
}

VisParamChangedClosure *visual_param_add_callback (VisParam *          self,
                                                   VisParamChangedFunc func,
                                                   void *              priv,
                                                   VisDestroyFunc      destroy_func)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (func != NULL, NULL);

    VisParamChangedClosure *closure = visual_mem_new0 (VisParamChangedClosure, 1);
    closure->func = func;
    closure->data = priv;
    closure->destroy_func = destroy_func;

    visual_list_add (self->changed_handlers, closure);

    return closure;
}

int visual_param_remove_callback (VisParam *self, VisParamChangedClosure *to_remove)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    VisListEntry *le = NULL;
    VisParamChangedClosure *closure;

    while ((closure = visual_list_next (self->changed_handlers, &le)) != NULL) {
        if (closure == to_remove) {
            visual_list_delete (self->changed_handlers, &le);

            if (closure->destroy_func) {
                closure->destroy_func (closure->data);
            }

            visual_mem_free (closure);

            return TRUE;
        }
    }

    return FALSE;
}

int visual_param_has_name (VisParam *self, const char *name)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    return strcmp (self->name, name);
}

void visual_param_changed (VisParam *self)
{
    visual_return_if_fail (self != NULL);

    if (!self->parent)
        return;

    VisEventQueue *eventqueue = self->parent->eventqueue;
    if (eventqueue) {
        visual_event_queue_add (eventqueue, visual_event_new_param (self));
    }

    visual_param_notify_callbacks (self);
}

void visual_param_notify_callbacks (VisParam *self)
{
    visual_return_if_fail (self != NULL);

    VisListEntry *le = NULL;
    VisParamChangedClosure *closure;

    while ((closure = visual_list_next (self->changed_handlers, &le)) != NULL)
        closure->func (self, closure->data);
}

VisParamType visual_param_get_type (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, VISUAL_PARAM_TYPE_NONE);

    return self->value.type;
}

const char *visual_param_get_name (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->name;
}

const char *visual_param_get_description (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->description;
}

const char *visual_param_get_value_string (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_STRING, 0);

    return self->value.value.string;
}

int visual_param_get_value_integer (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_INTEGER, 0);

    return self->value.value.integer;
}

float visual_param_get_value_float (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0.0f);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_FLOAT, 0.0f);

    return self->value.value.single_float;
}

double visual_param_get_value_double (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0.0);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_DOUBLE, 0.0);

    return self->value.value.double_float;
}

VisColor *visual_param_get_value_color (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_PALETTE, NULL);

    return self->value.value.color;
}

VisPalette *visual_param_get_value_palette (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (self->value.type != VISUAL_PARAM_TYPE_PALETTE, NULL);

    return self->value.value.palette;
}

VisParamValue *visual_param_value_new  (VisParamType type, void *value)
{
    VisParamValue *self = visual_mem_new0 (VisParamValue, 1);
    visual_param_value_set (self, value);
    return self;
}

void visual_param_value_copy (VisParamValue *self, VisParamValue *src)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_free_value (self);

    self->type = src->type;

    switch (src->type)
    {
        case VISUAL_PARAM_TYPE_NONE:
            break;
        case VISUAL_PARAM_TYPE_INTEGER:
        case VISUAL_PARAM_TYPE_FLOAT:
        case VISUAL_PARAM_TYPE_DOUBLE:
            self->value = self->value;
            break;
        case VISUAL_PARAM_TYPE_COLOR:
            //self->value.color = visual_color_clone (src->value.color);
            break;
        case VISUAL_PARAM_TYPE_STRING:
            self->value.string = visual_strdup (src->value.string);
            break;
        case VISUAL_PARAM_TYPE_PALETTE:
            //self->value.palette = visual_palette_clone (VISUAL_PALETTE (src->value.palette));
            break;
        default:
            break;
    }
}

void visual_param_value_set (VisParamValue *self, void *value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_free_value (self);

    switch (self->type)
    {
        case VISUAL_PARAM_TYPE_NONE:
            break;
        case VISUAL_PARAM_TYPE_STRING:
            self->value.string = visual_strdup ((const char *) value);
            break;
        case VISUAL_PARAM_TYPE_INTEGER:
            self->value.integer = (intptr_t) value;
            break;
        case VISUAL_PARAM_TYPE_FLOAT:
            self->value.single_float = (float) (intptr_t) value;
            break;
        case VISUAL_PARAM_TYPE_DOUBLE:
            self->value.double_float = *(double *) value;
            break;
        case VISUAL_PARAM_TYPE_COLOR:
            self->value.color = visual_color_new ();
            visual_color_set_with_alpha (self->value.color,
                                         *((uint8_t *) value + 0),
                                         *((uint8_t *) value + 1),
                                         *((uint8_t *) value + 2),
                                         *((uint8_t *) value + 3));
            break;
        case VISUAL_PARAM_TYPE_PALETTE:
            //self->value.palette = visual_palette_copy (VISUAL_PALETTE (value));
            break;
        default:
            break;
    }
}

void visual_param_value_free_value (VisParamValue *self)
{
    visual_return_if_fail (self != NULL);

    switch (self->type)
    {
        case VISUAL_PARAM_TYPE_STRING:
            visual_mem_free (self->value.string);
            break;
        case VISUAL_PARAM_TYPE_COLOR:
            visual_color_free (self->value.color);
            break;
        case VISUAL_PARAM_TYPE_PALETTE:
            visual_palette_free (self->value.palette);
            break;
        default:
            break;
    }

    self->type = VISUAL_PARAM_TYPE_NONE;
}

void visual_param_value_free (VisParamValue *self)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_free_value (self);
    visual_mem_free (self);
}

int visual_param_value_compare (VisParamValue *lhs, VisParamValue *rhs)
{
    visual_return_val_if_fail (lhs != NULL, FALSE);
    visual_return_val_if_fail (rhs != NULL, FALSE);

    if (lhs->type != rhs->type)
        return FALSE;

    switch (lhs->type) {
        case VISUAL_PARAM_TYPE_NONE:
            return TRUE;
        case VISUAL_PARAM_TYPE_STRING:
            return strcmp (lhs->value.string, rhs->value.string) == 0;
        case VISUAL_PARAM_TYPE_INTEGER:
            return lhs->value.integer == rhs->value.integer;
        case VISUAL_PARAM_TYPE_FLOAT:
            return lhs->value.single_float == rhs->value.single_float;
        case VISUAL_PARAM_TYPE_DOUBLE:
            return lhs->value.double_float == rhs->value.double_float;
        case VISUAL_PARAM_TYPE_COLOR:
            return visual_color_compare (lhs->value.color, rhs->value.color);
        case VISUAL_PARAM_TYPE_PALETTE:
            return FALSE;
        default:
            return FALSE;
    }
}
