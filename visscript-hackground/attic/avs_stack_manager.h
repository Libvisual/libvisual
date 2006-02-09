#ifndef _AVS_STACK_MANAGER_H
#define _AVS_STACK_MANAGER_H 1

struct _AvsStackManager;
typedef struct _AvsStackManager AvsStackManager;
struct _AvsStackManagerElement;
typedef struct _AvsStackManagerElement AvsStackManagerElement;

#define AVS_STACK_MANAGER(obj)			(VISUAL_CHECK_CAST ((obj), 0, AvsStackManager))

#define AVS_STACK_MANAGER_TEMPLATE(elem_type, prefix, name, initial_elem_count, max_elem_count) \
	prefix inline AvsStackManager * name##_new(void) { \
		return avs_stack_manager_new(sizeof(elem_type), initial_elem_count, max_elem_count); \
	} \
	prefix inline elem_type * name##_push(AvsStackManager *sm) { \
		return (elem_type *) avs_stack_manager_push(sm); \
	} \
	prefix inline int name##_pop(AvsStackManager *sm, void *elem) { \
		return avs_stack_manager_pop(sm, elem); \
	}

#define AVS_STACK_MANAGER_TEMPLATE_LOCAL(elem_type, name, initial_elem_count, max_elem_count) \
	AVS_STACK_MANAGER_TEMPLATE(elem_type, static, name, initial_elem_count, max_elem_count)
#define AVS_STACK_MANAGER_TEMPLATE_GLOBAL(elem_type, name, initial_elem_count, max_elem_count) \
	AVS_STACK_MANAGER_TEMPLATE(elem_type, , name, initial_elem_count, max_elem_count)

struct _AvsStackManager {
	VisObject		object;
	AvsBlobManager		*blob; /**< Avs Blob Manager used for memory management */
	AvsStackManagerElement	*pool; /**< AEP: Available Element Pool */
};

struct _AvsStackManagerElement {
	AvsStackManagerElement	*next;
	char data[];
};

/* prototypes */
void *avs_stack_manager_push(AvsStackManager *sm);
int avs_stack_manager_pop(AvsStackManager *sm, void *elem);
AvsStackManager *avs_stack_manager_new(int elem_size, int initial_elem_count, int max_elem_count);

#endif /* !AVS_STACK_MANAGER */
