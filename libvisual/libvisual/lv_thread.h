#ifndef _LV_THREAD_H
#define _LV_THREAD_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_common.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* FIXME check if pthread is supported, if not, #ifdef around pthread stuff */

typedef enum {
	VISUAL_THREAD_PRIORITY_LOW,
	VISUAL_THREAD_PRIORITY_NORMAL,
	VISUAL_THREAD_PRIORITY_HIGH,
	VISUAL_THREAD_PRIORITY_URGENT
} VisThreadPriority;

typedef struct _VisThread VisThread;
typedef struct _VisMutex VisMutex;

typedef void *(*VisThreadFunc)(void *);

struct _VisThread {
	pthread_t thread;
};

struct _VisMutex {
	pthread_mutex_t mutex;
};

int visual_thread_is_supported (void);
VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable);
int visual_thread_free (VisThread *thread);
void *visual_thread_join (VisThread *thread);
void visual_thread_exit (void *retval);
void visual_thread_yield (void);
VisThread *visual_thread_self (void);
void visual_thread_set_priority (VisThread *thread, VisThreadPriority priority);

VisMutex *visual_mutex_new (void);
int visual_mutex_lock (VisMutex *mutex);
int visual_mutex_trylock (VisMutex *mutex);
int visual_mutex_unlock (VisMutex *mutex);
int visual_mutex_free (VisMutex *mutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_THREAD_H */
