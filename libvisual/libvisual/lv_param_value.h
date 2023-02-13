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

#ifndef _LV_PARAM_VALUE_H
#define _LV_PARAM_VALUE_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_types.h>
#include <libvisual/lv_checks.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_palette.h>

#define VISUAL_PARAM_VALUE(obj) ((VisParamValue *) (obj))

typedef struct _VisParamValue VisParamValue;

typedef enum {
    VISUAL_PARAM_TYPE_NONE,
    VISUAL_PARAM_TYPE_BOOL,
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

LV_NODISCARD LV_API VisParamValue *visual_param_value_new (VisParamType type, void *value);

LV_API void visual_param_value_init       (VisParamValue *self, VisParamType type, void *value);
LV_API void visual_param_value_copy       (VisParamValue *value, VisParamValue *src);
LV_API void visual_param_value_set        (VisParamValue *value, VisParamType type, void *new_value);
LV_API int  visual_param_value_compare    (VisParamValue *lhs, VisParamValue *rhs);
LV_API void visual_param_value_free       (VisParamValue *value);
LV_API void visual_param_value_free_value (VisParamValue *value);

#define _LV_PARAM_MARSHAL_INTEGER(x)   ((void *) (intptr_t) (x))
#define _LV_PARAM_MARSHAL_FLOAT(x)     ((void *) (&x))
#define _LV_PARAM_MARSHAL_DOUBLE(x)    ((void *) (&x))
#define _LV_PARAM_MARSHAL_POINTER(x)   ((void *) (x))

#define _LV_DEFINE_PARAM_VALUE_SET(func,ctype,name,marshal) \
    static inline void visual_param_value_set_##func (VisParamValue *self, ctype value) {             \
        visual_return_if_fail (self != NULL);                                                         \
        visual_param_value_set (self, VISUAL_PARAM_TYPE_##name, _LV_PARAM_MARSHAL_##marshal (value)); \
    }

_LV_DEFINE_PARAM_VALUE_SET(bool   , int               , BOOL   , INTEGER)
_LV_DEFINE_PARAM_VALUE_SET(integer, int               , INTEGER, INTEGER)
_LV_DEFINE_PARAM_VALUE_SET(float  , float             , FLOAT  , FLOAT)
_LV_DEFINE_PARAM_VALUE_SET(double , double            , DOUBLE , DOUBLE)
_LV_DEFINE_PARAM_VALUE_SET(string , const char *      , STRING , POINTER)
_LV_DEFINE_PARAM_VALUE_SET(color  , const VisColor *  , COLOR  , POINTER)
_LV_DEFINE_PARAM_VALUE_SET(palette, const VisPalette *, PALETTE, POINTER)

#define _LV_DEFINE_PARAM_VALUE_GET(func,ctype,name,member,defvalue)                   \
    static inline ctype visual_param_value_get_##func (VisParamValue *self)  {        \
        visual_return_val_if_fail (self != NULL, 0);                                  \
        visual_return_val_if_fail (self->type == VISUAL_PARAM_TYPE_##name, defvalue); \
        return self->value.member;                                                    \
    }

_LV_DEFINE_PARAM_VALUE_GET(bool   , int         , BOOL   , integer     , FALSE)
_LV_DEFINE_PARAM_VALUE_GET(integer, int         , INTEGER, integer     , 0)
_LV_DEFINE_PARAM_VALUE_GET(float  , float       , FLOAT  , single_float, 0.0f)
_LV_DEFINE_PARAM_VALUE_GET(double , double      , DOUBLE , double_float, 0.0)
_LV_DEFINE_PARAM_VALUE_GET(string , const char *, STRING , string      , NULL)
_LV_DEFINE_PARAM_VALUE_GET(color  , VisColor *  , COLOR  , color       , NULL)
_LV_DEFINE_PARAM_VALUE_GET(palette, VisPalette *, PALETTE, palette     , NULL)

LV_END_DECLS

#endif /* LV_PARAM_VALUE_H */
