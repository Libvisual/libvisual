#include "config.h"
#include "lv_param.h"
#include "lv_common.h"

typedef struct {
    VisParamValue lower;
    VisParamValue upper;
} VisParamInRangeArgs;

static int  in_range_func      (VisParamValue *value, VisParamInRangeArgs *data);
static void in_range_args_free (VisParamInRangeArgs *args);

VisClosure *visual_param_in_range (VisParamType type, void *lower, void *upper)
{
    VisParamInRangeArgs *args = visual_mem_new0 (VisParamInRangeArgs, 1);
    visual_param_value_set (&args->lower, type, lower);
    visual_param_value_set (&args->upper, type, upper);

    return visual_closure_new (in_range_func, args, (VisDestroyFunc) in_range_args_free);
}

int in_range_func (VisParamValue *value, VisParamInRangeArgs *data)
{
    visual_return_val_if_fail (value != NULL, FALSE);
    visual_return_val_if_fail (data  != NULL, FALSE);

    switch (value->type) {
        case VISUAL_PARAM_TYPE_INTEGER: {
            int v = visual_param_value_get_integer (value);
            int a = visual_param_value_get_integer (&data->lower);
            int b = visual_param_value_get_integer (&data->upper);
            return (v >= a && v <= b);
        }
        case VISUAL_PARAM_TYPE_FLOAT: {
            float v = visual_param_value_get_float (value);
            float a = visual_param_value_get_float (&data->lower);
            float b = visual_param_value_get_float (&data->upper);
            return (v >= a && v <= b);
        }
        case VISUAL_PARAM_TYPE_DOUBLE: {
            double v = visual_param_value_get_double (value);
            double a = visual_param_value_get_double (&data->lower);
            double b = visual_param_value_get_double (&data->upper);
            return (v >= a && v <= b);
        }
        default: {
            return FALSE;
        }
    }
}

void in_range_args_free (VisParamInRangeArgs *args)
{
    visual_param_value_free_value (&args->lower);
    visual_param_value_free_value (&args->upper);
    visual_mem_free (args);
}
