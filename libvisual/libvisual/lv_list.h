#ifndef _LV_LIST_H
#define _LV_LIST_H

#include <libvisual/lv_common.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/queue.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _VisListEntry VisListEntry;
typedef struct _VisList VisList;

/**
 * The VisListEntry data structure is an entry within the linked list.
 * It does contain a pointer to both the previous and next entry in the list and
 * a void * to the data.
 */
struct _VisListEntry {
	VisListEntry		*prev;	/**< Previous entry in the list. */
	VisListEntry		*next;	/**< Next entry in the list. */
	void			*data;	/**< Pointer to the data for this entry. */
};

/**
 * The VisList data structure holds the linked list.
 * It contains an entry pointer to both the head and tail of the list as well
 * an entry counter.
 */
struct _VisList {
	VisListEntry		*head;	/**< Pointer to the beginning of the list. */
	VisListEntry		*tail;	/**< Pointer to the end of the list. */
	int			count;	/**< Number of entries that are in the list. */
};

typedef int (*visual_list_sort_func_t)(void*); /* FIXME this is the prototype we want? */
typedef void (*visual_list_destroy_func_t)(void*);

/* prototypes */
VisList *visual_list_new (void);
int visual_list_free (VisList *list);
int visual_list_destroy (VisList *list, visual_list_destroy_func_t destroyer);

inline void *visual_list_next(VisList *list, VisListEntry **le);
inline void *visual_list_prev(VisList *list, VisListEntry **le);

void *visual_list_get (VisList *list, int index);

int visual_list_add_at_begin (VisList *list, void *data);
int visual_list_add (VisList *list, void *data);

int visual_list_insert (VisList *list, VisListEntry **le, void *data);
int visual_list_delete (VisList *list, VisListEntry **le);

int visual_list_count (VisList *list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LIST_H */
