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
#include "lv_param_value.h"
#include "lv_common.h"
#include "lv_util.h"
#include <string.h>

VisParamValue *visual_param_value_new  (VisParamType type, void *value)
{
    VisParamValue *self = visual_mem_new0 (VisParamValue, 1);
    visual_param_value_init (self, type, value);
    return self;
}

void visual_param_value_init (VisParamValue *self, VisParamType type, void *value)
{
    self->type = VISUAL_PARAM_TYPE_NONE;
    visual_param_value_set (self, type, value);
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
        case VISUAL_PARAM_TYPE_BOOL:
        case VISUAL_PARAM_TYPE_INTEGER:
        case VISUAL_PARAM_TYPE_FLOAT:
        case VISUAL_PARAM_TYPE_DOUBLE:
            self->value = self->value;
            break;
        case VISUAL_PARAM_TYPE_COLOR:
            self->value.color = visual_color_clone (src->value.color);
            break;
        case VISUAL_PARAM_TYPE_STRING:
            self->value.string = visual_strdup (src->value.string);
            break;
        case VISUAL_PARAM_TYPE_PALETTE:
            self->value.palette = visual_palette_clone (src->value.palette);
            break;
        default:
            break;
    }
}

void visual_param_value_set (VisParamValue *self, VisParamType type, void *value)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (type != VISUAL_PARAM_TYPE_NONE);

    visual_param_value_free_value (self);

    self->type = type;

    switch (type)
    {
        case VISUAL_PARAM_TYPE_NONE:
            break;
        case VISUAL_PARAM_TYPE_BOOL:
        case VISUAL_PARAM_TYPE_INTEGER:
            self->value.integer = (intptr_t) value;
            break;
        case VISUAL_PARAM_TYPE_FLOAT: {
            self->value.single_float = *(float *) value;
            break;
        }
        case VISUAL_PARAM_TYPE_DOUBLE:
            self->value.double_float = *(double *) value;
            break;
        case VISUAL_PARAM_TYPE_STRING:
            self->value.string = visual_strdup ((const char *) value);
            break;
        case VISUAL_PARAM_TYPE_COLOR:
            self->value.color = visual_color_clone (value);
            break;
        case VISUAL_PARAM_TYPE_PALETTE:
            self->value.palette = visual_palette_clone (value);
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
        case VISUAL_PARAM_TYPE_BOOL:
        case VISUAL_PARAM_TYPE_INTEGER:
            return lhs->value.integer == rhs->value.integer;
        case VISUAL_PARAM_TYPE_FLOAT:
            return lhs->value.single_float == rhs->value.single_float;
        case VISUAL_PARAM_TYPE_DOUBLE:
            return lhs->value.double_float == rhs->value.double_float;
        case VISUAL_PARAM_TYPE_STRING:
            return strcmp (lhs->value.string, rhs->value.string) == 0;
        case VISUAL_PARAM_TYPE_COLOR:
            return visual_color_compare (lhs->value.color, rhs->value.color);
        case VISUAL_PARAM_TYPE_PALETTE:
            return FALSE;
        default:
            return FALSE;
    }
}
