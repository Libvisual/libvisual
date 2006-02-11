#ifndef _AVS_STACK_H
#define _AVS_STACK_H 1

struct _AvsStack;
typedef struct _AvsStack AvsStack;

#define AVS_STACK(obj)			(VISUAL_CHECK_CAST ((obj), AvsStack))

#define AVS_STACK_TEMPLATE(elem_type, prefix, name, initial_elem_count, max_elem_count) \
	prefix inline AvsStack * name##_new(void) { \
		return avs_stack_new(sizeof(elem_type), initial_elem_count, max_elem_count); \
	} \
	prefix inline elem_type * name##_push(AvsStack *stack) { \
		return (elem_type *) avs_stack_push(stack); \
	} \
	prefix inline elem_type * name##_pop(AvsStack *stack) { \
		return (elem_type *) avs_stack_pop(stack); \
	}

#define AVS_STACK_TEMPLATE_LOCAL(elem_type, name, initial_elem_count, max_elem_count) \
	AVS_STACK_TEMPLATE(elem_type, static, name, initial_elem_count, max_elem_count)
#define AVS_STACK_TEMPLATE_GLOBAL(elem_type, name, initial_elem_count, max_elem_count) \
	AVS_STACK_TEMPLATE(elem_type, , name, initial_elem_count, max_elem_count)

#define avs_stack_new0(elem_type, initial_elem_count, max_elem_count) \
	avs_stack_new(sizeof(elem_type), initial_elem_count, max_elem_count)

struct _AvsStack {
	VisObject	object;
	unsigned char	*base;
	unsigned char	*end;
	unsigned char	*pointer;
	int		elem_size;
	int		elem_count;
	int		elem_max;
};

#define avs_stack_begin(sp)	(void *)(sp)->base
#define avs_stack_end(sp) 	(void *)(sp)->pointer

/* prototypes */
void avs_stack_reset(AvsStack *sp);
int avs_stack_resize(AvsStack *sp);
int avs_stack_move(AvsStack *sp, int direction);
void *avs_stack_multipush(AvsStack *sp, int count);
void *avs_stack_push(AvsStack *sp);
void *avs_stack_multipop(AvsStack *sp, int count);
void *avs_stack_pop(AvsStack *sp);
AvsStack *avs_stack_new(int elem_size, int initial_elem_count, int max_elem_count);

#endif /* !_AVS_STACK_H */
