
#include "avs_globals.h"

/*
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
*/

int global_proxy_dtor(VisObject *obj)
{
    int i;
    AvsGlobalProxy *proxy = AVS_GLOBAL_PROXY(obj);

    if(proxy->buffers != NULL) {
        for(i = 0; i < MAXBUF; i++) {
            if(proxy->buffers[i].buffer != NULL)
                visual_mem_free(proxy->buffers[i].buffer);
        }
        visual_mem_free(proxy->buffers);
    }

    visual_object_unref(VISUAL_OBJECT(proxy->multidelay));

    visual_mem_free(proxy->framebuffer);
    visual_mem_free(proxy->fbout);

    for(i = 0; i < MAXBUF; i++) {
        visual_mem_free(proxy->buffers[i].buffer);
    }

    visual_video_free_buffer(proxy->framebuffer);
    visual_video_free_buffer(proxy->fbout);

    visual_mem_free(proxy);

    return FALSE;
}

int multidelay_dtor(VisObject *obj)
{
    AvsMultidelayGlobals *multidelay = AVS_MULTIDELAY(obj);
    int i;

    for(i = 0; i < MAXBUF; i++)
    {
        visual_mem_free(multidelay->buffer[i].fb);
    }

    visual_mem_free(multidelay);

    return VISUAL_OK;
}

int multidelay_init(AvsMultidelayGlobals *multidelay)
{
    int i;
    for(i = 0; i < MAXBUF; i++)
    {
        multidelay->renderid = 0;
        multidelay->framessincebeat = 0;
        multidelay->framesperbeat = 0;
        multidelay->framemem = 1;
        multidelay->oldframemem = 1;
        multidelay->usebeats[i] = FALSE;
        multidelay->delay[i] = 0;
        multidelay->framedelay[i] = 0;
        multidelay->buffersize[i] = 1;
        multidelay->virtualbuffersize[i] = 1;
        multidelay->oldvirtualbuffersize[i] = 1;
        multidelay->buffer[i].fb = visual_mem_malloc0(multidelay->buffersize[i]);
        multidelay->inpos[i].fb = multidelay->buffer[i].fb;
        multidelay->outpos[i].fb = multidelay->buffer[i].fb;
    }

    return VISUAL_OK;
}

AvsGlobalProxy *avs_global_proxy_new(int w, int h, int depth) 
{
    AvsGlobalProxy *proxy = visual_mem_new0(AvsGlobalProxy, 1);
    
    visual_object_initialize(VISUAL_OBJECT(proxy), 0, global_proxy_dtor);

    proxy->multidelay = visual_mem_new0(AvsMultidelayGlobals, 1);

    proxy->framebuffer = visual_video_new_with_buffer(w, h, depth);

    proxy->fbout = visual_video_new_with_buffer(w, h, depth);

    visual_object_initialize(VISUAL_OBJECT(proxy->multidelay), 0, multidelay_dtor);

    multidelay_init(proxy->multidelay);

    return proxy;
}

int avs_global_proxy_resize(AvsGlobalProxy *proxy, int w, int h, int depth)
{
	VisVideo *tmp = proxy->framebuffer;
	proxy->framebuffer = visual_video_scale_depth_new(tmp, w, h, depth, VISUAL_VIDEO_SCALE_NEAREST);
	visual_video_free_buffer(tmp);
	tmp = proxy->fbout;
	proxy->fbout = visual_video_scale_depth_new(tmp, w, h, depth, VISUAL_VIDEO_SCALE_NEAREST);
	visual_video_free_buffer(tmp);
	return 0;
}
