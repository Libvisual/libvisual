#ifndef _LV_LIBVISUAL_H
#define _LV_LIBVISUAL_H

#include <libvisual/lv_param.h>
#include <libvisual/lv_ui.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char *visual_get_version (void);
VisParamContainer *visual_get_params (void);
VisUIWidget *visual_get_userinterface (void);
int visual_init_path_add (char *pathadd);
int visual_init (int *argc, char ***argv);
int visual_is_initialized (void);
int visual_quit (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LIBVISUAL_H */
