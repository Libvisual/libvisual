#ifndef _LV_LIBVISUAL_H
#define _LV_LIBVISUAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libvisual/lv_param.h>

const char *visual_get_version (void);
VisParamContainer *visual_get_params (void);
int visual_init_path_add (char *pathadd);
int visual_init (int *argc, char ***argv);
int visual_quit (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LIBVISUAL_H */
