/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Chong Kai Xiong <kaixiong@codeleft.sg>
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

#ifndef _LV_PARAM_VALUE_H
#define _LV_PARAM_VALUE_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_checks.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_palette.h>

#define VISUAL_PARAM_VALUE(obj) ((VisParamValue *) (obj))

typedef struct _VisParamValue VisParamValue;

typedef enum {
    VISUAL_PARAM_TYPE_NONE,
    VISUAL_PARAM_TYPE_INTEGER,
    VISUAL_PARAM_TYPE_FLOAT,
    VISUAL_PARAM_TYPE_DOUBLE,
    VISUAL_PARAM_TYPE_COLOR,
    VISUAL_PARAM_TYPE_STRING,
    VISUAL_PARAM_TYPE_PALETTE,
} VisParamType;

struct _VisParamValue
{
    VisParamType type;

    union {
        int          integer;
        float        single_float;
        double       double_float;
        char *       string;
        VisColor *   color;
        VisPalette * palette;
    } value;
};

LV_BEGIN_DECLS

LV_API VisParamValue *visual_param_value_new        (VisParamType type, void *value);
LV_API void           visual_param_value_copy       (VisParamValue *value, VisParamValue *src);
LV_API void           visual_param_value_set        (VisParamValue *value, VisParamType type, void *new_value);
LV_API int            visual_param_value_compare    (VisParamValue *lhs, VisParamValue *rhs);
LV_API void           visual_param_value_free       (VisParamValue *value);
LV_API void           visual_param_value_free_value (VisParamValue *value);

#define _LV_PARAM_MARSHAL_INTEGER(x)   ((void *) (intptr_t) (x))
#define _LV_PARAM_MARSHAL_FLOAT(x)     ((void *) (intptr_t) *(int32_t *) &x)
#define _LV_PARAM_MARSHAL_DOUBLE(x)    (&x)
#define _LV_PARAM_MARSHAL_POINTER(x)   ((void *) (x))

static inline void visual_param_value_set_integer (VisParamValue *self, int value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_INTEGER, _LV_PARAM_MARSHAL_INTEGER (value));
}

static inline void visual_param_value_set_float (VisParamValue *self, float value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_FLOAT, _LV_PARAM_MARSHAL_FLOAT (value));
}

static inline void visual_param_value_set_double (VisParamValue *self, double value)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_DOUBLE, _LV_PARAM_MARSHAL_DOUBLE (value));
}

static inline void visual_param_value_set_string (VisParamValue *self, const char *string)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_STRING, _LV_PARAM_MARSHAL_POINTER (string));
}

static inline void visual_param_value_set_color (VisParamValue *self, VisColor *color)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_COLOR, _LV_PARAM_MARSHAL_POINTER (color));
}

static inline void visual_param_value_set_palette (VisParamValue *self, VisPalette *palette)
{
    visual_return_if_fail (self != NULL);

    visual_param_value_set (self, VISUAL_PARAM_TYPE_PALETTE, _LV_PARAM_MARSHAL_POINTER (palette));
}

static inline const char *visual_param_value_get_string (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, 0);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_STRING, 0);

    return self->value.string;
}

static inline int visual_param_value_get_integer (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, 0);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_INTEGER, 0);

    return self->value.integer;
}

static inline float visual_param_value_get_float (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, 0.0f);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_FLOAT, 0.0f);

    return self->value.single_float;
}

static inline double visual_param_value_get_double (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, 0.0);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_DOUBLE, 0.0);

    return self->value.double_float;
}

static inline VisColor *visual_param_value_get_color (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_COLOR, NULL);

    return self->value.color;
}

static inline VisPalette *visual_param_value_get_palette (VisParamValue *self)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (self->type != VISUAL_PARAM_TYPE_PALETTE, NULL);

    return self->value.palette;
}

LV_END_DECLS

#endif /* LV_PARAM_VALUE_H */
