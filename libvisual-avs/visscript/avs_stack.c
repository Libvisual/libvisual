#include <stdio.h>
#include <stdlib.h>

#include "avs.h"

static inline int resize_stack(AvsStack *sp)
{
	int offset;

	if (sp->elem_count >= sp->elem_max)
		return -1;
	
	sp->elem_count *= 2;
	if (sp->elem_count >= sp->elem_max)
		sp->elem_count = sp->elem_max;

	avs_debug(print("stack: resizing stack: %p to %d elements of %d byte(s) each.", 
			sp, sp->elem_count, sp->elem_size));

	offset = sp->pointer - sp->base;
	if ((sp->base = realloc(sp->base, sp->elem_count * sp->elem_size)) == NULL)
		return -1;

	sp->end = sp->base + sp->elem_count * sp->elem_size;
	sp->pointer = sp->base + offset;
	return 0;
}

static inline int request_elem(AvsStack *sp, int count)
{
	for (;;) {
		if (sp->pointer + (count - 1) * sp->elem_size < sp->end)
			return 0;
		
		if (resize_stack(sp))
			return -1;
	}

	return -1;
}

/**
 * 	Reset stack pointer to zero, effectively clearing the stack.
 *
 * 	@param sp Stack context.
 *
 * 	@returns Nothing.
 */
void avs_stack_reset(AvsStack *sp)
{
	sp->pointer = sp->base;
}

/**
 * 	Increase stack size.
 *
 * 	@param sp Stack context.
 *
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL when the maximum allowed stack
 * 	size has been reached.
 */
int avs_stack_resize(AvsStack *sp)
{
	if (resize_stack(sp))
		return VISUAL_ERROR_GENERAL;

	return VISUAL_OK;
}

/**
 *	Move stack pointer, checking for bounds along the way.
 *
 *	@param sp Stack context.
 *	@param direction Number of elements to move stack pointer. May be negative to 
 *			 move the stackpointer into the opposite direction.
 *			 
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_stack_move(AvsStack *sp, int direction)
{
	unsigned char *np = sp->pointer + direction * sp->elem_size;

	if (np < sp->base)
		return VISUAL_ERROR_GENERAL;

	/* Check if more elements are requested 
	 * than the stack allows */
	if (np >= sp->end && request_elem(sp, direction))
		return VISUAL_ERROR_GENERAL;
	
	sp->pointer = np;
	return VISUAL_OK;
}

/**
 *	Push multiple stack values onto the stack at once.
 *	Zeroing them in the process.
 *
 *	@param sp Stack context.
 *	@param count Number of stack values to push.
 *	
 *	@returns Pointer to first pushed stack value on success, NULL 
 *	when the maximum allowable stack elements is reached.
 */
void * avs_stack_multipush(AvsStack *sp, int count)
{
	void *retval;
	
	if (request_elem(sp, count))
		return NULL; /* OOM/Maximum number of elements reached */

	retval = sp->pointer;
	sp->pointer += sp->elem_size * count;
	memset(retval, 0, sp->elem_size * count);

	return retval;
}

/**
 *	Push a single stack value onto the stack.
 *	Zeroing the stack value in the process.
 *
 *	@param sp Stack context.
 *
 *	@returns Pointer to pushed stack value on success, NULL 
 *	when the maximum allowable stack elements is reached.
 */
void * avs_stack_push(AvsStack *sp)
{
	void *retval;
	
	if (request_elem(sp, 1))
		return NULL; /* OOM/Maximum number of elements reached */

	retval = sp->pointer;
	sp->pointer += sp->elem_size;
	memset(retval, 0, sp->elem_size);

	return retval;
}

/**
 *	Pop multiple stack values from the stack.
 *	Returning a pointer to the first stack value in the array.
 *
 *	@param sp Stack context.
 *	@param count Number of items to pop.
 *	
 *	@return Pointer to first stack value in the array, NULL when no stack
 *	values are available.
 */ 
void * avs_stack_multipop(AvsStack *sp, int count)
{
	if (sp->pointer - (count - 1) * sp->elem_size <= sp->base)
		return NULL; /* No elements available */
	
	sp->pointer -= count * sp->elem_size;
	return sp->pointer;
}

/**
 *	Pop a single stack value from the stack.
 *
 *	@param sp Stack context.
 *
 *	@return Pointer to popped stack value on success, NULL 
 *	when no stack elements are available.
 */ 
void * avs_stack_pop(AvsStack *sp)
{
	if (sp->pointer <= sp->base)
		return NULL; /* No elements available */
	
	sp->pointer -= sp->elem_size;
	return sp->pointer;
}

static int stack_dtor(VisObject * object)
{
	AvsStack *obj = AVS_STACK(object);
	visual_mem_free(obj->base);
	return VISUAL_OK;
}

/**
 *	Create a new general-use resizable stack.
 *
 *	@param elem_size Size of precisely one element.
 *	@param initial_elem_count Initial size of the stack.
 *	@param max_elem_count Maximum allowable size of the stack.
 *
 *	@returns A newly allocated stack on success, NULL on failure.
 */
AvsStack * avs_stack_new(int elem_size, int initial_elem_count, int max_elem_count)
{
	AvsStack *sp;
	
	sp = visual_mem_new0(AvsStack, 1);
	visual_object_initialize(VISUAL_OBJECT(sp), TRUE, stack_dtor);

	/* Initialize stack structure */
	sp->elem_size = elem_size;
	sp->elem_count = initial_elem_count;
	sp->elem_max = max_elem_count;

	/* Allocate initial stack space */
	sp->base = visual_mem_malloc0(sp->elem_size * sp->elem_count);
	sp->end = sp->base + sp->elem_count * sp->elem_size;
	sp->pointer = sp->base;
	
	return sp;
}

