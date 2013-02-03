/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include <map>
#include <list>
#include <cstdarg>
#include <cstring>

typedef std::map<std::string, VisParam*> ParamEntries;
typedef std::list<VisClosure*>           HandlerList;

struct _VisClosure
{
    void           (*func) (void);
    void *         data;
    VisDestroyFunc destroy_func;
};

struct _VisParam
{
    char *         name;
    char *         description;
    VisParamValue  value;
    VisParamValue  default_value;
    VisClosure *   validator;
    HandlerList    changed_handlers;
    VisParamList * parent;
};

struct _VisParamList {
    ParamEntries    entries;
    VisEventQueue * eventqueue;
};

static inline int validate_param_value (VisParamValue *value, VisClosure *validator)
{
    return (* (VisParamValidateFunc) validator->func) (value, validator->data);
}

VisClosure *visual_closure_new (void *func, void *data, VisDestroyFunc destroy_func)
{
    VisClosure *self = visual_mem_new0 (VisClosure, 1);

    self->func = reinterpret_cast<void(*)(void)> (func);
    self->data = data;
    self->destroy_func = destroy_func;

    return self;
}

void visual_closure_free (VisClosure *self)
{
    if (!self)
        return;

    if (self->data && self->destroy_func)
        self->destroy_func (self->data);

    visual_mem_free (self);
}

VisParamList *visual_param_list_new (void)
{
    auto self = new VisParamList;

    self->eventqueue = nullptr;

    return self;
}

void visual_param_list_free (VisParamList *self)
{
    if (!self) {
        return;
    }

    for (auto& entry : self->entries) {
        visual_param_free (entry.second);
    }

    delete self;
}

void visual_param_list_set_event_queue (VisParamList *self, VisEventQueue *eventqueue)
{
    visual_return_if_fail (self != NULL);

    self->eventqueue = eventqueue;
}

VisEventQueue *visual_param_list_get_event_queue (VisParamList *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->eventqueue;
}

VisParam** visual_param_list_get_entries (VisParamList *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    //return self->entries;
    return NULL;
}

int visual_param_list_add (VisParamList *self, VisParam *param)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (param != NULL, FALSE);

    auto old_param = self->entries[param->name];
    if (old_param) {
        visual_param_free (old_param);
    }

    self->entries[param->name] = param;

    return FALSE;
}

unsigned int visual_param_list_add_array (VisParamList *self, VisParam **params, unsigned int nparams)
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

unsigned int visual_param_list_add_many (VisParamList *self, ...)
{
    va_list args;

    va_start (args, self);

    VisParam *param;
    unsigned int added = 0;

    do {
        param = va_arg (args, VisParam *);
        if (param && visual_param_list_add (self, param)) {
            added++;
        }
    } while (param);

    va_end (args);

    return added;
}

int visual_param_list_remove (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (name != NULL, FALSE);

    auto entry = self->entries.find (name);
    if (entry != self->entries.end ()) {
        visual_param_free (entry->second);
        self->entries.erase (entry);
        return TRUE;
    }

    return FALSE;
}

VisParam *visual_param_list_get (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (name != NULL, NULL);

    auto entry = self->entries.find (name);
    if (entry != self->entries.end ()) {
        return entry->second;
    }

    return NULL;
}

VisParam *visual_param_new (const char * name,
                            const char * description,
                            VisParamType type,
                            void *       default_value,
                            VisClosure * validator)
{
    visual_return_val_if_fail (name != NULL, NULL);
    visual_return_val_if_fail (description != NULL, NULL);
    visual_return_val_if_fail (type != VISUAL_PARAM_TYPE_NONE, NULL);

    if (validator) {
        VisParamValue value;
        visual_param_value_init (&value, type, default_value);
        visual_return_val_if_fail (validate_param_value (&value, validator), NULL);
    }

    auto self = new VisParam;

    self->name        = visual_strdup (name);
    self->description = visual_strdup (description);
    self->validator   = validator;

    visual_param_value_set (&self->value, type, default_value);
    visual_param_value_set (&self->default_value, type, default_value);

    return self;
}

void visual_param_free (VisParam *self)
{
    if (!self)
        return;

    visual_mem_free (self->name);
    visual_mem_free (self->description);

    visual_param_value_free_value (&self->value);
    visual_param_value_free_value (&self->default_value);

    visual_closure_free (self->validator);

    for (auto closure : self->changed_handlers) {
        visual_closure_free (closure);
    }

    delete self;
}

VisClosure *visual_param_add_callback (VisParam *          self,
                                       VisParamChangedFunc func,
                                       void *              priv,
                                       VisDestroyFunc      destroy_func)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (func != NULL, NULL);

    VisClosure *closure = visual_closure_new (reinterpret_cast<void*> (func), priv, destroy_func);
    self->changed_handlers.push_back (closure);

    return closure;
}

int visual_param_remove_callback (VisParam *self, VisClosure *to_remove)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    auto entry = std::find (self->changed_handlers.begin (),
                            self->changed_handlers.end (),
                            to_remove);

    if (entry != self->changed_handlers.end ()) {
        visual_closure_free (*entry);
        self->changed_handlers.erase (entry);
        return TRUE;
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

    for (auto closure : self->changed_handlers) {
        (*(VisParamChangedFunc) (closure->func)) (self, closure->data);
    }
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

void visual_param_set_value_bool (VisParam *self, int value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_bool (&self->value, value);
}

void visual_param_set_value_integer (VisParam *self, int value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_integer (&self->value, value);
}

void visual_param_set_value_float (VisParam *self, float value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_float (&self->value, value);
}

void visual_param_set_value_double (VisParam *self, double value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_double (&self->value, value);
}

void visual_param_set_value_string (VisParam *self, const char *string)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_string (&self->value, string);
}

void visual_param_set_value_color (VisParam *self, VisColor *color)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_color (&self->value, color);
}

void visual_param_set_value_palette (VisParam *self, VisPalette *palette)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set_palette (&self->value, palette);
}

int visual_param_get_value_bool (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return visual_param_value_get_bool (&self->value);
}

int visual_param_get_value_integer (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return visual_param_value_get_integer (&self->value);
}

float visual_param_get_value_float (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0.0f);

    return visual_param_value_get_float (&self->value);
}

double visual_param_get_value_double (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0.0);

    return visual_param_value_get_double (&self->value);
}

const char *visual_param_get_value_string (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return visual_param_value_get_string (&self->value);
}

VisColor *visual_param_get_value_color (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return visual_param_value_get_color (&self->value);
}

VisPalette *visual_param_get_value_palette (VisParam *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return visual_param_value_get_palette (&self->value);
}
