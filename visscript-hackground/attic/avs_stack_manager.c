#include <stdio.h>
#include <stdlib.h>

#include "avs.h"

/* FIXME: Severely broken!!!
 * 	After an stack resize all pointers could possibly be
 * 	pointing to a chunk of invalid memory.
 *
 */

/**
 *	Push a single stack value onto the stack.
 *	And zero stack value.
 *
 *	@param sp Stack manager context.
 *
 *	@returns Pointer to pushed stack value on success, NULL 
 *	when the maximum allowable stack elements is reached.
 */
void * avs_stack_manager_push(AvsStackManager *sm)
{
	AvsStackManagerElement *elem;
	
	/* First try to recycle used stack elements.
	 * By looking into Available Element Pool */
	if (sm->pool) {
		elem = sm->pool;
		sm->pool = elem->next;
	} else {
		/* No used stack elements to recycle
		 * Allocate a new stack element */
		elem = avs_stack_push(sm->stack);
	}

	if (!elem)
		return NULL;

	memset(elem->data, 0, sm->stack->elem_size - sizeof(AvsStackManagerElement));
	return elem->data;
}

/**
 *	Pop a single stack value from the stack.
 *	This function should not be compared with the normal
 *	avs_stack_pop(). This function doesn't actually pop the value from the underlying
 *	stack, but destroys the stack element and adds it to the stackmanager's 
 *	Available Element Pool instead. 
 *	After popping the passed element it is NOT useable anymore.
 *
 *	@param sp Stack manager context.
 *	@param elem Element to pop from the stack.
 * 	
 * 	@note The element given by elem SHOULD NOT be used anymore after popping.
 *
 * 	@return On success VISUAL_OK, VISUAL_ERROR_GENERAL on failure.
 */ 
int avs_stack_manager_pop(AvsStackManager *sm, void *elem)
{
	AvsStackManagerElement *sme = (AvsStackManagerElement *)(elem - sizeof(AvsStackManagerElement));

	/* Add element to the Available Element Pool */
	if (sm->pool) 
		sme->next = sm->pool;
	else
		sme->next = NULL;
	
	sm->pool = sme;
	return VISUAL_OK;
}

static int manager_dtor(VisObject * object)
{
	AvsStackManager *sm = AVS_STACK_MANAGER(object);
	visual_object_unref(VISUAL_OBJECT(sm->stack));
	return VISUAL_OK;
}

/**
 * 	Create a new stack manager.
 * 	Uses an avs stack as the underlying stack.
 *	
 *	@param elem_size Size of precisely one element.
 *	@param initial_elem_count Initial size of the stack.
 *	@param max_elem_count Maximum allowable size of the stack.
 *
 * 	@see avs_stack_new
 *	@returns A newly allocated stack manager on success, NULL on failure.
 */
AvsStackManager *avs_stack_manager_new(int elem_size, int max_elem_count)
{
	AvsStackManager *sm;

	sm = visual_mem_new0(AvsStackManager, 1);
	visual_object_initialize(VISUAL_OBJECT(sm), TRUE, manager_dtor);

	/* Allocate stack.
	 * Increase the element size so that our AvsStackManagerElement can fit
	 * in front of normal stack elements. */
	elem_size += sizeof(AvsStackManagerElement);
	sm->stack = avs_stack_new(elem_size, initial_elem_count, max_elem_count);

	/* Initialize AEP */
	sm->pool = NULL;

	return sm;
}
