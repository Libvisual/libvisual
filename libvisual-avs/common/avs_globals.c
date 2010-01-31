
#include "avs_globals.h"

AvsGlobalBuffer *avs_get_global_buffer(AvsGlobalProxy *obj, int w, int h, int n, int do_alloc)
{
    if(n < 0 || n >= MAXBUF)
        return NULL;

    if(obj->buffers == NULL) {
        obj->buffers = visual_mem_new0(AvsGlobalBuffer, MAXBUF);
    }
    
    if(!obj->buffers[n].buffer || obj->buffers[n].w != w || obj->buffers[n].h != h)
    {
        if(obj->buffers[n].buffer) {
            visual_mem_free(obj->buffers[n].buffer);
        }
        if(do_alloc) {
            obj->buffers[n].w = w;
            obj->buffers[n].h = h;
            obj->buffers[n].buffer = visual_mem_new0(uint32_t, w * h);
            return &obj->buffers[n];
        }

        obj->buffers[n].buffer = NULL;
        obj->buffers[n].w = 0;
        obj->buffers[n].h = 0;

        return NULL;
    }

    return &obj->buffers[n];
}

int global_proxy_dtor(VisObject *obj)
{
    AvsGlobalProxy *proxy = AVS_GLOBAL_PROXY(obj);

    if(proxy->buffers != NULL) {
        int i;
        for(i = 0; i < MAXBUF; i++) {
            if(proxy->buffers[i].buffer != NULL)
                visual_mem_free(proxy->buffers[i].buffer);
        }
        visual_mem_free(proxy->buffers);
    }
    visual_mem_free(proxy);

    return FALSE;
}

AvsGlobalProxy *avs_global_proxy_new() 
{
    AvsGlobalProxy *proxy = visual_mem_new0(AvsGlobalProxy, 1);
    visual_object_initialize(VISUAL_OBJECT(proxy), 1, global_proxy_dtor);
    return proxy;
}
