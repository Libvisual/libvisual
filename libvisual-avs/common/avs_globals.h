
#ifndef _GLOBAL_PROXY_H_
#define _GLOBAL_PROXY_H_

#include <libvisual/libvisual.h>

#define AVS_GLOBAL_PROXY(obj)   VISUAL_CHECK_CAST((obj), AVSGlobalProxy)

#define MAXBUF 8

typedef struct {
    uint32_t *buffer;
    int w;
    int h;
} AVSGlobalBuffer;

typedef struct {
    VisObject obj;
    unsigned char   blendtable[256][256];
    int reset_vars_on_recompile;
    int line_blend_mode;
    AVSGlobalBuffer *buffers;
    int numbuffers;
    char path[];
} AVSGlobalProxy;

AVSGlobalBuffer *avs_get_global_buffer(AVSGlobalProxy *obj, int w, int h, int n, int do_alloc);
AVSGlobalProxy *avs_global_proxy_new();

#endif
