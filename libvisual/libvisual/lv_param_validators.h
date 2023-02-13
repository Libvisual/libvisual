#ifndef _LV_PARAM_VALIDATORS_H
#define _LV_PARAM_VALIDATORS_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_param_value.h>

#ifdef __cplusplus

namespace LV {

    class Closure;

} // LV namespace

typedef LV::Closure VisClosure;

#else

typedef struct _VisClosure VisClosure;

#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisClosure *visual_param_in_range (VisParamType type, void *lower, void *upper);

#define _LV_DEFINE_PARAM_IN_RANGE(func,ctype,type,marshal) \
    static inline VisClosure *visual_param_in_range_##func (ctype lower, ctype upper) { \
        return visual_param_in_range (VISUAL_PARAM_TYPE_##type,                         \
                                      _LV_PARAM_MARSHAL_##marshal (lower),              \
                                      _LV_PARAM_MARSHAL_##marshal (upper));             \
    }

_LV_DEFINE_PARAM_IN_RANGE (integer, int   , INTEGER, INTEGER)
_LV_DEFINE_PARAM_IN_RANGE (float  , float , FLOAT,   FLOAT)
_LV_DEFINE_PARAM_IN_RANGE (double , double, DOUBLE,  DOUBLE)

LV_END_DECLS

#endif /* _LV_PARAM_VALIDATORS_H */
