/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_string.c,v 1.1 2006-09-27 22:19:58 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <gettext.h>

#include <lvconfig.h>
#include "lv_log.h"
#include "lv_mem.h"
#include "lv_mem_pool.h"
#include "lv_string.h"

typedef struct _StringIteratorAll StringIteratorAll;
typedef struct _StringIteratorToken StringIteratorToken;
typedef struct _StringIteratorStringToken StringIteratorStringToken;

struct _StringIteratorAll {
	VisObject		object;

	int			index;
};

/* Keep the StringIteratorAll in the top of the struct is important, we do casts! */
struct _StringIteratorToken {
	StringIteratorAll	iterator;

	char			token;
};

struct _StringIteratorStringToken {
	VisObject		object;

	int			index;

	VisString		token;

	unsigned char		cycled;
};

/* Internal variables */
static VisMemPool __lv_string_mempool;
static int __lv_string_initialized = FALSE;


static int string_dtor (VisObject *object);
static int string_iterator_dtor (VisObject *object);

static int link_delink_childs (VisString *str);
static VisString *link_select_new_master (VisString *str);
static int link_get_ready_for_new_content (VisString *str);
static int link_maybe_new_child (VisString *str1, VisString *str2);
inline static void link_make_relations (VisString *parent, VisString *child);

static int should_realloc_to_get_smaller (int size, int newsize);

static int strings_are_the_same (VisString *str1, VisString *str2);

inline static int string_make_buffer_ready (VisString *str, visual_size_t n);
inline static int string_terminate (VisString *str, int index);

static int string_copy (VisString *dest, VisString *src, visual_size_t n);

static int string_modify_range_additive (VisString *str, char min, char max, int additive);

static int string_iterator_all_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_all_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_all_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_all_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken);

static int string_iterator_token_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken);

static int string_iterator_token_set_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_set_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_set_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_token_set_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken);

static int string_iterator_string_token_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_string_token_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_string_token_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
static int string_iterator_string_token_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken);


static int string_dtor (VisObject *object)
{
	VisString *str = VISUAL_STRING (object);

	if (str->buffer != NULL && str->childs != NULL)
		link_select_new_master (str);

	if (str->buffer != NULL)
		visual_object_unref (VISUAL_OBJECT (str->buffer));

	if (str->childs != NULL) {
		visual_collection_destroy (VISUAL_COLLECTION (str->childs));

		visual_object_unref (VISUAL_OBJECT (str->childs));
	}

	if (str->parententry != NULL && str->parent != NULL)
		visual_list_delete (str->parent->childs, &str->parententry);

	str->buffer = NULL;
	str->childs = NULL;
	str->parententry = NULL;
	str->parent = NULL;

	if ((str->flags & VISUAL_STRING_FLAG_IN_POOL) > 0) {
		visual_mem_pool_free (&__lv_string_mempool, str);

		return FALSE;
	}

	return TRUE;
}

static int string_iterator_dtor (VisObject *object)
{
	VisStringIterator *siterator = VISUAL_STRING_ITERATOR (object);
	VisObject *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	visual_object_unref (VISUAL_OBJECT (&siterator->string));

	if (ctx != NULL)
		visual_object_unref (ctx);

	visual_object_set_private (VISUAL_OBJECT (siterator), NULL);

	return TRUE;
}

static int string_iterator_token_dtor (VisObject *object)
{
	VisStringIteratorToken *sitoken = VISUAL_STRING_ITERATOR_TOKEN (object);

	if (sitoken->iterator != NULL)
		visual_object_unref (VISUAL_OBJECT (sitoken->iterator));

	sitoken->iterator = NULL;

	return TRUE;
}

static int propagate_new_buffer (VisString *str)
{
        VisListEntry *le = NULL;
	VisString *cs;

	while ((cs = visual_list_next (str->childs, &le)) != NULL) {
		visual_object_unref (VISUAL_OBJECT (cs->buffer));
		cs->buffer = str->buffer;
		visual_object_ref (VISUAL_OBJECT (str->buffer));

		propagate_new_buffer (cs);
	}

	return VISUAL_OK;
}

static int link_delink_childs (VisString *str)
{
	VisString *nm;

	nm = link_select_new_master (str);

	if (nm != NULL) {
		VisBuffer *buf = visual_buffer_new_allocate (str->length + 1, visual_buffer_destroyer_free);
		visual_buffer_put_length (buf, str->buffer, 0, str->length + 1);

		visual_object_unref (VISUAL_OBJECT (str->buffer));

		nm->buffer = buf;

		propagate_new_buffer (nm);
	}

	return VISUAL_OK;
}

static VisString *link_select_new_master (VisString *str)
{
	VisListEntry *le = NULL;
	VisList *cl;
	VisString *nm;
	VisString *cs;

	if (visual_collection_size (VISUAL_COLLECTION (str->childs)) <= 0)
		return NULL;

	/* List manipulation hacking following! (Not using API
	 * for performance reasons) */
	nm = VISUAL_STRING (str->childs->head->data);
	cl = nm->childs;
	nm->parent = NULL;

	if (cl->tail != NULL)
		cl->tail->next = str->childs->head->next;
	else {
		cl->tail = str->childs->head->next;
		cl->head = cl->tail;
	}

	cl->count += str->childs->count - 1;

	/* Freeing the head entry, because the head is now parent */
	visual_mem_free (str->childs->head);

	str->childs->head = NULL;
	str->childs->tail = NULL;
	str->childs->count = 0;

	while ((cs = visual_list_next (cl, &le)) != NULL) {
		cs->parent = nm;
	}

	return nm;
}

static int link_get_ready_for_new_content (VisString *str)
{
	/* Get ready for content */
	if (str->buffer != NULL) {
		if (visual_collection_size (VISUAL_COLLECTION (str->childs)) > 0) {

			link_select_new_master (str);

			visual_object_unref (VISUAL_OBJECT (str->buffer));

			str->buffer = NULL;

		} else if (visual_object_refcount (VISUAL_OBJECT (str->buffer)) > 1) {

			visual_object_unref (VISUAL_OBJECT (str->buffer));

			str->buffer = NULL;
		}
	}

	return VISUAL_OK;
}

static int link_maybe_new_child (VisString *str1, VisString *str2)
{
	if ((str1->flags & VISUAL_STRING_FLAG_FORCE_COPY) > 0 && (str2->flags & VISUAL_STRING_FLAG_FORCE_COPY) > 0) {
		if (str1->buffer != NULL)
			visual_object_unref (VISUAL_OBJECT (str1->buffer));

		str1->buffer = str2->buffer;

		visual_object_ref (VISUAL_OBJECT (str1->buffer));

		link_make_relations (str2, str1);

		propagate_new_buffer (str1);
	}

	return VISUAL_OK;
}

inline static void link_make_relations (VisString *parent, VisString *child)
{
	visual_list_add (parent->childs, child);

	child->parententry = parent->childs->tail;
	child->parent = parent;
}

static int should_realloc_to_get_smaller (int size, int newsize)
{
	return TRUE;
}

static int strings_are_the_same (VisString *str1, VisString *str2)
{
	if (str1->buffer == str2->buffer &&
			str1->length == str2->length)
		return TRUE;

	return FALSE;
}

inline static int string_make_buffer_ready (VisString *str, visual_size_t n)
{
	if (str->buffer == NULL) {
		str->buffer = visual_buffer_new_allocate (n, visual_buffer_destroyer_free);
	} else if (should_realloc_to_get_smaller (str->buffer->datasize, n) ||
			str->buffer->datasize < n) {

		visual_object_unref (VISUAL_OBJECT (str->buffer));

		str->buffer = visual_buffer_new_allocate (n, visual_buffer_destroyer_free);
	}

	return VISUAL_OK;
}

inline static int string_terminate (VisString *str, int index)
{
	char *s;

	s = (char *) visual_string_get_cstring (str);

	s[index] = '\0';

	return VISUAL_OK;
}

static int string_copy (VisString *dest, VisString *src, visual_size_t n)
{
	link_get_ready_for_new_content (dest);

	if (dest->buffer == NULL) {
		dest->buffer = visual_buffer_new_allocate (n + 1, visual_buffer_destroyer_free);

		visual_buffer_put_length (dest->buffer, src->buffer, 0, n);
		string_terminate (dest, n);

	} else {
		string_make_buffer_ready (dest, n);

		visual_buffer_put_length (dest->buffer, src->buffer, 0, n);
		string_terminate (dest, n);
	}

	return VISUAL_OK;
}

static int string_modify_range_additive (VisString *str, char min, char max, int additive)
{
	char *s;
	int i;
	int len;
	int changed = FALSE;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	s = (char *) visual_string_get_cstring (str);

	len = visual_string_length (str);

	for (i = 0; i < len; i++) {
		if (s[i] >= min && s[i] <= max) {
			if (changed == FALSE) {
				link_select_new_master (str);

				s = (char *) visual_string_get_cstring (str);

				changed = TRUE;
			}

			s[i] += additive;
		}
	}

	return VISUAL_OK;
}

inline static void string_iterator_token_pair_make_ready (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	if (*sitoken == NULL)
		*sitoken = visual_string_iterator_token_new (siterator);

	if ((*sitoken)->iterator != siterator) {
		if ((*sitoken)->iterator != NULL)
			visual_object_unref (VISUAL_OBJECT ((*sitoken)->iterator));

		(*sitoken)->iterator = siterator;
		visual_object_ref (VISUAL_OBJECT ((*sitoken)->iterator));
	}
}

static int string_iterator_all_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorAll *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	if (ctx->index >= siterator->string.length)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	if (ctx->index < 0)
		ctx->index = 0;

	string_iterator_token_pair_make_ready (siterator, sitoken);

	(*sitoken)->token.character = visual_string_get_cstring (&siterator->string)[ctx->index];
	(*sitoken)->index = ctx->index;

	ctx->index++;

	return VISUAL_OK;
}

static int string_iterator_all_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorAll *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	if (ctx->index < 0 && siterator->string.length > 0)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	string_iterator_token_pair_make_ready (siterator, sitoken);

	if (ctx->index >= siterator->string.length)
		ctx->index = siterator->string.length - 1;

	(*sitoken)->token.character = visual_string_get_cstring (&siterator->string)[ctx->index];
	(*sitoken)->index = ctx->index;

	ctx->index--;

	return VISUAL_OK;
}

static int string_iterator_all_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorAll *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	if (siterator->string.length <= 0)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	string_iterator_token_pair_make_ready (siterator, sitoken);

	(*sitoken)->token.character = visual_string_get_cstring (&siterator->string)[0];
	(*sitoken)->index = 0;

	ctx->index++;

	return VISUAL_OK;
}

static int string_iterator_all_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorAll *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	if (siterator->string.length <= 0)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	string_iterator_token_pair_make_ready (siterator, sitoken);

	(*sitoken)->token.character = visual_string_get_cstring (&siterator->string)[siterator->string.length - 1];
	(*sitoken)->index = siterator->string.length - 1;

	ctx->index--;

	return VISUAL_OK;
}

static int string_iterator_token_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	char current;
	int index;

	current = (*sitoken)->token.character;
	index = (*sitoken)->index;

	// FIXME use strchr here (or the visstring equalevant !
	while (string_iterator_all_next (siterator, sitoken) == VISUAL_OK) {
		if ((*sitoken)->token.character == ctx->token)
			return VISUAL_OK;
	}

	(*sitoken)->token.character = current;
	(*sitoken)->index = index;

	return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;
}

static int string_iterator_token_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	char current;
	int index;

	current = (*sitoken)->token.character;
	index = (*sitoken)->index;

	while (string_iterator_all_prev (siterator, sitoken) == VISUAL_OK) {
		if ((*sitoken)->token.character == ctx->token)
			return VISUAL_OK;
	}

	(*sitoken)->token.character = current;
	(*sitoken)->index = index;

	return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;
}

static int string_iterator_token_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	char current;
	int index;

	current = (*sitoken)->token.character;
	index = (*sitoken)->index;

	if (string_iterator_all_first (siterator, sitoken) == VISUAL_OK) {
		while (string_iterator_all_next (siterator, sitoken) == VISUAL_OK) {
			if ((*sitoken)->token.character == ctx->token)
				return VISUAL_OK;
		}
	}

	(*sitoken)->token.character = current;
	(*sitoken)->index = index;

	return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;
}

static int string_iterator_token_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	char current;
	int index;

	current = (*sitoken)->token.character;
	index = (*sitoken)->index;

	if (string_iterator_all_last (siterator, sitoken) == VISUAL_OK) {
		while (string_iterator_all_prev (siterator, sitoken) == VISUAL_OK) {
			if ((*sitoken)->token.character == ctx->token)
				return VISUAL_OK;
		}
	}

	(*sitoken)->token.character = current;
	(*sitoken)->index = index;

	return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;
}

static int string_iterator_token_set_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{

}

static int string_iterator_token_set_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{

}

static int string_iterator_token_set_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{

}

static int string_iterator_token_set_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{

}

static int string_iterator_string_token_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorStringToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	char *s;

	if (ctx->cycled == FALSE)
		ctx->cycled = TRUE;
	else
		(*sitoken)->index += visual_string_length (&ctx->token);

	if ((*sitoken)->index + visual_string_length (&ctx->token) >= siterator->string.length)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	if ((*sitoken)->index < 0)
		(*sitoken)->index = 0;

	string_iterator_token_pair_make_ready (siterator, sitoken);

	s = strstr (visual_string_get_cstring (&siterator->string) + (*sitoken)->index,
			visual_string_get_cstring (&ctx->token));

	if (s == NULL)
		return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;

	/* First set the index, our char pointer will possibly get invalid after a visual_string_copy */
	(*sitoken)->index = s - visual_string_get_cstring (&siterator->string);
	visual_string_copy (&(*sitoken)->token.string, &ctx->token);

	return VISUAL_OK;
}

static int string_iterator_string_token_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorStringToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	return -VISUAL_ERROR_STRING_ITERATOR_NO_ENTRIES;
	return VISUAL_OK;
}

static int string_iterator_string_token_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorStringToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	return VISUAL_OK;
}

static int string_iterator_string_token_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	StringIteratorStringToken *ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	return VISUAL_OK;
}


/**
 * @defgroup VisString VisString
 * @{
 */

int visual_string_initialize ()
{
	visual_mem_pool_tls_init (&__lv_string_mempool, sizeof (VisString), 16, NULL, TRUE);
	visual_mem_pool_create (&__lv_string_mempool);

	__lv_string_initialized = TRUE;

	return VISUAL_OK;
}

int visual_string_is_initialized ()
{
	return __lv_string_initialized;
}

int visual_string_deinitialize ()
{
	if (__lv_string_initialized == FALSE)
		return -VISUAL_ERROR_STRING_NOT_INITIALIZED;

	visual_object_unref (VISUAL_OBJECT (&__lv_string_mempool));

	__lv_string_initialized = FALSE;

	return VISUAL_OK;
}

/**
 * Allocates a new empty VisString.
 *
 * @return The newly allocated VisString.
 */
VisString *visual_string_new ()
{
	VisString *str;

	str = visual_mem_new0 (VisString, 1);

	visual_string_init (str);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (str), TRUE);
	visual_object_ref (VISUAL_OBJECT (str));

	return str;
}

/**
 * Allocates a new VisString with a value.
 *
 * @param s The value that is put in the newly allocataed VisString.
 *
 * @return The newly allocated VisString containing the value.
 */
VisString *visual_string_new_with_value (const char *s)
{
	VisString *str;

	str = visual_string_new ();

	visual_string_set (str, s);

	return str;
}

VisString *visual_string_new_parameter_string (const char *s)
{
	VisString *str = NULL;
	int len;

	if (s != NULL) {
		str = visual_mem_pool_alloc (&__lv_string_mempool);
		visual_string_init_force_copy (str, TRUE);
		visual_object_ref (VISUAL_OBJECT (str));

		len = strlen (s);

		/* Be aware that a parameter string should never be written to. The reason is that
		 * data can live within a read only segment */
		str->buffer = visual_buffer_new_with_buffer ((void *) s, len, NULL);
		str->length = len;

		str->flags |= VISUAL_STRING_FLAG_IN_POOL;
		str->flags |= VISUAL_STRING_FLAG_PARAMETER;
	}

	return str;
}

int visual_string_init (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_init_force_copy (str, FALSE);
}

int visual_string_init_force_copy (VisString *str, int force_copy)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (str));
	visual_object_set_dtor (VISUAL_OBJECT (str), string_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (str), FALSE);

	/* Set the VisString data */
	visual_object_clean (VISUAL_OBJECT (str), VisString);

	if (force_copy > 0) {
		visual_string_force_duplication (str, TRUE);
	} else {
		str->childs = visual_list_new (visual_object_collection_destroyer);
	}

	return VISUAL_OK;
}

int visual_string_ref_parameter (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if ((str->flags & VISUAL_STRING_FLAG_PARAMETER) > 0) {
		visual_object_ref (VISUAL_OBJECT (str));
	}

	return VISUAL_OK;
}

int visual_string_unref_parameter (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if ((str->flags & VISUAL_STRING_FLAG_PARAMETER) > 0) {
		visual_object_unref (VISUAL_OBJECT (str));
	}

	return VISUAL_OK;
}

/**
 * Gives the standard const char* c string from a VisString. The value returned is always read only, you
 * should never write to the const char* buffer that is returned. Also the const char* address is not constant
 * for a VisString, so you should not keep a reference to this pointer, especially not between operations on
 * VisStrings.
 *
 * @see visual_string_get_buffer
 *
 * @param str Pointer to the VisString of which the const char* c string is requested.
 *
 * @return The read only const char* c string that is encapsulated within the VisString.
 */
const char *visual_string_get_cstring (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, NULL);

	if (str->buffer == NULL)
		return NULL;

	return visual_buffer_get_data (str->buffer);
}

/**
 * Gives the VisBuffer containing the string data from a VisString. The value returned is always read only, you
 * should never write to the VisBuffer that is returned. Also the VisBuffer address is not constant
 * for a VisString, so you should not keep a reference to this pointer, especially not between operations on
 * VisStrings.
 *
 * @see visual_string_get_cstring
 *
 * @param str Pointer to the VisString of which the const char* c string is requested.
 *
 * @return The read only VisBuffer containing the string data that is encapsulated within the VisString.
 */
VisBuffer *visual_string_get_buffer (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, NULL);

	return str->buffer;
}

int visual_string_force_duplication (VisString *str, int force_copy)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (force_copy > 0) {
		if (str->childs != NULL) {
			link_delink_childs (str);

			visual_object_unref (VISUAL_OBJECT (str->childs));

			str->childs = NULL;
		}

		str->flags |= VISUAL_STRING_FLAG_FORCE_COPY;

	} else if (str->flags & VISUAL_STRING_FLAG_FORCE_COPY) {
		if (str->childs == NULL)
			str->childs = visual_list_new (visual_object_collection_destroyer);

		str->flags -= VISUAL_STRING_FLAG_FORCE_COPY;
	}

	return VISUAL_OK;
}

int visual_string_copy (VisString *dest, VisString *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	if (strings_are_the_same (dest, src) == TRUE)
		return VISUAL_OK;

	if ((src->flags & VISUAL_STRING_FLAG_FORCE_COPY) > 0) {
		string_copy (dest, src, src->length);
	} else {
		if (dest->buffer != NULL) {
			link_select_new_master (dest);

			visual_object_unref (VISUAL_OBJECT (dest->buffer));
		}

		dest->buffer = src->buffer;

		visual_object_ref (VISUAL_OBJECT (src->buffer));

		link_make_relations (src, dest);
	}

	dest->length = src->length;

	return VISUAL_OK;
}

int visual_string_n_copy (VisString *dest, VisString *src, visual_size_t n)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	if (src->length == n) {
		visual_string_copy (dest, src);
	} else {
		string_copy (dest, src, n);
	}

	return VISUAL_OK;
}

int visual_string_copy_value (char *dest, VisString *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src->buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	return visual_buffer_copy_data_to_length (src->buffer, dest, src->length + 1);
}

int visual_string_n_copy_value (char *dest, VisString *src, visual_size_t n)
{
	return visual_string_n_copy_value_offset (dest, src, 0, n);
}

int visual_string_n_copy_value_offset (char *dest, VisString *src, int offset, visual_size_t n)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src->buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (n <= 0)
		return VISUAL_OK;

	visual_buffer_copy_data_to_offset_length (src->buffer, dest, offset, n - 1);

	dest[n - 1] = '\0';

	return VISUAL_OK;
}

int visual_string_length (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, 0);

	return str->length;
}

int visual_string_substring (VisString *dest, VisString *src, int begin, int end)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	/* Complete string, so make a reference */
	if (begin == 0 && end == src->length) {
		visual_string_copy (dest, src);
	} else {
		link_select_new_master (dest);

		if (begin < 0)
			begin = 0;

		if (end < 0)
			end = 0;

		if (begin >= src->length) {
			visual_string_set (dest, "");

			return VISUAL_OK;
		}

		if (end >= src->length)
			end = src->length - 1;

		/* FIXME: handle the zero length string case */

		string_make_buffer_ready (dest, (end - begin) + 2);

		visual_buffer_put_data (dest->buffer,
				((char *) visual_buffer_get_data (src->buffer)) + begin,
				(end - begin) + 1,
				0);

		string_terminate (dest, (end - begin) + 1);

		dest->length = (end - begin) + 1;
	}

	return VISUAL_OK;
}

int visual_string_substring_begin (VisString *dest, VisString *src, int begin)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_substring (dest, src, begin, src->length - begin);
}

int visual_string_substring_end (VisString *dest, VisString *src, int end)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_substring (dest, src, src->length - end, end);
}

VisString *visual_string_set (VisString *str, const char *s)
{
	VisString wrap;
	int len = strlen (s);

	visual_log_return_val_if_fail (str != NULL, str);

	if (str->buffer == NULL) {
		str->buffer = visual_buffer_new_allocate (len + 1, visual_buffer_destroyer_free);
	} else {
		link_select_new_master (str);

		string_make_buffer_ready (str, len + 1);
	}

	visual_buffer_put_data (str->buffer, (char *) s, len + 1, 0);

	str->length = len;

	return str;
}

int visual_string_set_many (VisString *str, const char *s, ...)
{
	va_list ap;
	void *va_entry;
	VisString *va_str;
	const char *va_s;
	int is_s = FALSE;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (s != NULL, -VISUAL_ERROR_NULL);

	visual_string_set (str, s);

	va_start (ap, s);

	while ((va_entry = va_arg (ap, void *)) != NULL) {
		if (is_s = 0) {
			va_str = va_entry;

			is_s = TRUE;
		} else {
			va_s = va_entry;

			visual_string_set (va_str, va_s);

			is_s = FALSE;
		}
	}

	if (is_s == TRUE) {
		visual_log (VISUAL_LOG_CRITICAL, _("VisString count is not equal to const char* count."));
	}

	va_end (ap);

	return VISUAL_OK;
}

int visual_string_combine (VisString *dest, VisString *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_STRING_NULL);

	if (dest->buffer == NULL) {
		visual_string_copy (dest, src);
	} else if (dest->buffer != NULL && src->buffer != NULL) {
		visual_size_t len;

		len = dest->length + src->length;

		/* Dest is already large enough for both, get ready for content */
		if (visual_buffer_get_size (dest->buffer) >= len + 1) {
			link_select_new_master (dest);

			visual_buffer_put (dest->buffer, src->buffer, dest->length);

			dest->length = len;
		} else {
			VisString temp;

			visual_string_init (&temp);

			string_make_buffer_ready (&temp, len + 1);

			visual_buffer_put (temp.buffer, dest->buffer, 0);
			visual_buffer_put (temp.buffer, src->buffer, dest->length);

			temp.length = len;

			/* FIXME: Make a copy variant that uses dest as head (and use
			 * this on temp variables which will have to switch parentness anytime soon */
			visual_string_copy (dest, &temp);

			visual_object_unref (VISUAL_OBJECT (&temp));
		}
	}

	return VISUAL_OK;
}

int visual_string_remove (VisString *str, int begin, int end)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);


}

int visual_string_fill (VisString *str, char c, visual_size_t n)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (str->buffer == NULL) {
		str->buffer = visual_buffer_new_allocate (n + 1, visual_buffer_destroyer_free);
	} else {
		link_select_new_master (str);

		string_make_buffer_ready (str, n + 1);
	}

	visual_mem_set (visual_buffer_get_data (str->buffer), c, n);
	string_terminate (str, n);

	str->length = n;

	return VISUAL_OK;
}

int visual_string_compare (VisString *str1, VisString *str2)
{
	const char *s1;
	const char *s2;
	int ret = 0;

	visual_log_return_val_if_fail (str1 != NULL, 0);
	visual_log_return_val_if_fail (str2 != NULL, 0);

	/* FIXME make maxsize and minsize defines for types and use maxsize here, since
	 * we recheck it anyway in n_compare */
	ret = visual_string_n_compare (str1, str2, VISUAL_MIN (str1->length, str2->length));

	s1 = visual_string_get_cstring (str1);
	s2 = visual_string_get_cstring (str2);

	if (str1->length > str2->length) {
		ret = s1[str2->length] - s2[str2->length];
	} else if (str1->length < str2->length) {
		ret = s1[str1->length] - s2[str1->length];
	}

	return ret;
}

int visual_string_n_compare (VisString *str1, VisString *str2, visual_size_t n)
{
	const char *s1;
	const char *s2;
	int ret = 0;
	int i;

	visual_log_return_val_if_fail (str1 != NULL, 0);
	visual_log_return_val_if_fail (str2 != NULL, 0);

	if (strings_are_the_same (str1, str2) == TRUE)
		return 0;

	s1 = visual_string_get_cstring (str1);
	s2 = visual_string_get_cstring (str2);

	n = VISUAL_MIN (VISUAL_MIN (str1->length, str2->length), n);

	/* FIXME create super fast memcompare using mmx and stuff in VisMem */
	/* Compare */
	for (i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			ret = s1[i] - s2[i];

			break;
		}
	}

	/* Strings are the same, link! */
	if (ret == 0 && str1->length == str2->length)
		link_maybe_new_child (str1, str2);

	return ret;
}

int visual_string_case_compare (VisString *str1, VisString *str2)
{
	visual_log_return_val_if_fail (str1 != NULL, 0);
	visual_log_return_val_if_fail (str2 != NULL, 0);

	/* FIXME make maxsize and minsize defines for types and use maxsize here, since
	 * we recheck it anyway in n_compare */
	return visual_string_n_compare (str1, str2, VISUAL_MIN (str1->length, str2->length));
}

int visual_string_n_case_compare (VisString *str1, VisString *str2, visual_size_t n)
{
	const char *s1;
	const char *s2;
	int ret = 0;
	int i;

	visual_log_return_val_if_fail (str1 != NULL, 0);
	visual_log_return_val_if_fail (str2 != NULL, 0);

	if (strings_are_the_same (str1, str2) == TRUE)
		return 0;

	s1 = visual_string_get_cstring (str1);
	s2 = visual_string_get_cstring (str2);

	n = VISUAL_MIN (VISUAL_MIN (str1->length, str2->length), n);

	/* Compare */
	for (i = 0; i < n; i++) {
		if (tolower (s1[i]) != tolower (s2[i])) {
			ret = tolower (s1[i]) - tolower (s2[i]);

			break;
		}
	}

	/* Strings are the same, link! */
	if (ret == 0 && str1->length == str2->length)
		link_maybe_new_child (str1, str2);

	return ret;
}

int visual_string_ends_with (VisString *str1, VisString *str2)
{
	return visual_string_starts_with_offset (str1, str2, visual_string_length (str1) - visual_string_length (str2));
}

int visual_string_starts_with (VisString *str1, VisString *str2)
{
	return visual_string_starts_with_offset (str1, str2, 0);
}

int visual_string_starts_with_offset (VisString *str1, VisString *str2, int offset)
{
	return visual_string_find_string_offset (str1, str2, 0) == 0 ? TRUE : FALSE;
}

int visual_string_find_char (VisString *str, char c)
{
	return visual_string_find_char_offset (str, c, 0);
}

int visual_string_find_char_offset (VisString *str, char c, int offset)
{
	char *s;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset >= visual_string_length (str))
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	s = strchr (visual_string_get_cstring (str) + offset, c);

	if (s == NULL)
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	return s - visual_string_get_cstring (str);
}

int visual_string_reverse_find_char (VisString *str, char c)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

}

int visual_string_find_char_set (VisString *str, VisString *set)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (set != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_find_char_set_offset (str, set, 0);
}

int visual_string_find_char_set_offset (VisString *str, VisString *set, int offset)
{
	const char *h;
	const char *s;
	int hlen;
	int slen;
	int i;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (set != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset >= visual_string_length (str));
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	h = visual_string_get_cstring (str) + offset;
	s = visual_string_get_cstring (set);

	hlen = visual_string_length (str) - offset;
	slen = visual_string_length (set);

	while (hlen--) {
		for (i = 0; i < slen; i++) {
			if (s[i] == *h) {
				return h - visual_string_get_cstring (str);
			}
		}

		h++;
	}

	return -VISUAL_ERROR_STRING_NOT_FOUND;
}

int visual_string_reverse_find_char_set (VisString *str, VisString *set)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (set != NULL, -VISUAL_ERROR_STRING_NULL);

}

int visual_string_find_string (VisString *haystack, VisString *needle)
{
	visual_log_return_val_if_fail (haystack != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (needle != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_find_string_offset (haystack, needle, 0);
}

int visual_string_find_string_offset (VisString *haystack, VisString *needle, int offset)
{
	char *s;

	visual_log_return_val_if_fail (haystack != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (needle != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset + visual_string_length (needle) >= visual_string_length (haystack))
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	s = strstr (visual_string_get_cstring (haystack) + offset, visual_string_get_cstring (needle));

	if (s == NULL) {
		return -VISUAL_ERROR_STRING_NOT_FOUND;
	}

	return s - visual_string_get_cstring (haystack);
}

int visual_string_find_last_string (VisString *haystack, VisString *needle)
{
	visual_log_return_val_if_fail (haystack != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (needle != NULL, -VISUAL_ERROR_STRING_NULL);
}

int visual_string_find_last_string_offset (VisString *haystack, VisString *needle, int offset)
{
	visual_log_return_val_if_fail (haystack != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (needle != NULL, -VISUAL_ERROR_STRING_NULL);
}

int visual_string_replace (VisString *str, char oldchar, char newchar)
{
	return visual_string_replace_offset (str, oldchar, newchar, 0);
}

int visual_string_replace_offset (VisString *str, char oldchar, char newchar, int offset)
{
	int len;
	int i;
	int firstchar;
	char *s;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset >= visual_string_length (str))
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	firstchar = visual_string_find_char_offset (str, oldchar, offset);

	if (firstchar < 0)
		return -VISUAL_ERROR_STRING_NOT_FOUND;

	len = visual_string_length (str);

	link_select_new_master (str);

	string_make_buffer_ready (str, len);

	s = (char *) visual_string_get_cstring (str);

	for (i = 0; i < len; i++) {
		if (s[i] == oldchar)
			s[i] = newchar;
	}

	string_terminate (str, len);

	return VISUAL_OK;
}

int visual_string_replace_string (VisString *str, VisString *oldstr, VisString *newstr)
{
	return visual_string_replace_string_offset (str, oldstr, newstr, 0);
}

int visual_string_replace_string_offset (VisString *str, VisString *oldstr, VisString *newstr, int offset)
{
	char *s;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (oldstr != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (newstr != NULL, -VISUAL_ERROR_STRING_NULL);


}

int visual_string_to_lower_case (VisString *str)
{
	return string_modify_range_additive (str, 'A', 'Z', 32);
}

int visual_string_to_upper_case (VisString *str)
{
	return string_modify_range_additive (str, 'a', 'z', -32);
}

int visual_string_trim (VisString *str)
{
	const char *s;
	int i;
	int trimcntb;
	int trimcnte;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	s = visual_string_get_cstring (str);

	trimcntb = visual_string_count_char_sequencial (str, ' ', 0);
	trimcnte = visual_string_count_char_sequencial_tail (str, ' ', 0);

	if (trimcntb > 0) {
		link_select_new_master (str);


//		visual_buffer_put_

	}

	return VISUAL_OK;
}

int visual_string_count_char_sequencial (VisString *str, char c, int offset)
{
	const char *s;
	int cnt = 0;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset >= visual_string_length (str))
		return 0;

	s = visual_string_get_cstring (str) + offset;

	while (*s == c) {
		cnt++;

		s++;
	}

	return cnt;
}

int visual_string_count_char_sequencial_tail (VisString *str, char c, int offset)
{
	const char *s;
	int cnt = 0;

	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	if (offset >= visual_string_length (str))
		return 0;

	s = visual_string_get_cstring (str) + visual_string_length (str) + offset;

	while (*s == c) {
		cnt++;

		s--;
	}

	return cnt;
}

uint32_t visual_string_get_hashcode (VisString *str)
{
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	return visual_string_get_hashcode_cstring (visual_string_get_cstring (str));
}

/* X31 HASH found in g_str_hash */
uint32_t visual_string_get_hashcode_cstring (const char *s)
{
	uint32_t hash = 0;

	visual_log_return_val_if_fail (s != NULL, -VISUAL_ERROR_NULL);

	for (; *s != '\0'; *s++)
		hash = (hash << 5) - hash  + *s;

	return hash;
}

VisStringIterator *visual_string_iterator_new (VisString *str)
{
	VisStringIterator *siterator;

	siterator = visual_string_iterator_new_custom (str, VISUAL_STRING_ITERATOR_TYPE_NONE, NULL, NULL, NULL, NULL, NULL);

	return siterator;
}

VisStringIterator *visual_string_iterator_new_all (VisString *str)
{
	VisStringIterator *siterator;

	siterator = visual_mem_new0 (VisStringIterator, 1);

	visual_string_iterator_init_all (siterator, str);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (siterator), TRUE);
	visual_object_ref (VISUAL_OBJECT (siterator));

	return siterator;
}

int visual_string_iterator_init_all (VisStringIterator *siterator, VisString *str)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	visual_string_iterator_init (siterator, VISUAL_STRING_ITERATOR_TYPE_CHARACTER, str,
			string_iterator_all_first,
			string_iterator_all_next,
			string_iterator_all_prev,
			string_iterator_all_last,
			VISUAL_OBJECT (visual_object_new_and_clean (StringIteratorAll)));

	return VISUAL_OK;
}

VisStringIterator *visual_string_iterator_new_token (VisString *str, char token)
{
	VisStringIterator *siterator;

	siterator = visual_mem_new0 (VisStringIterator, 1);

	visual_string_iterator_init_token (siterator, str, token);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (siterator), TRUE);
	visual_object_ref (VISUAL_OBJECT (siterator));

	return siterator;
}

int visual_string_iterator_init_token (VisStringIterator *siterator, VisString *str, char token)
{
	StringIteratorToken *ctx;

	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	visual_string_iterator_init (siterator, VISUAL_STRING_ITERATOR_TYPE_CHARACTER, str,
			string_iterator_token_first,
			string_iterator_token_next,
			string_iterator_token_prev,
			string_iterator_token_last,
			VISUAL_OBJECT (visual_object_new_and_clean (StringIteratorToken)));

	ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
	ctx->token = token;

	return VISUAL_OK;
}

// FIXME afmaken
VisStringIterator *visual_string_iterator_new_token_set (VisString *str, VisString *tokenset)
{
	VisStringIterator *siterator;

	siterator = visual_mem_new0 (VisStringIterator, 1);

	visual_string_iterator_init_token_set (siterator, str, tokenset);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (siterator), TRUE);
	visual_object_ref (VISUAL_OBJECT (siterator));

	return siterator;
}

// FIXME afmaken
int visual_string_iterator_init_token_set (VisStringIterator *siterator, VisString *str, VisString *tokenset)
{
	StringIteratorToken *ctx;

	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	visual_string_iterator_init (siterator, VISUAL_STRING_ITERATOR_TYPE_CHARACTER, str,
			string_iterator_token_set_first,
			string_iterator_token_set_next,
			string_iterator_token_set_prev,
			string_iterator_token_set_last,
			VISUAL_OBJECT (visual_object_new_and_clean (StringIteratorToken)));

	ctx = visual_object_get_private (VISUAL_OBJECT (siterator));
//	ctx->token = token;

	return VISUAL_OK;
}

VisStringIterator *visual_string_iterator_new_string_token (VisString *str, VisString *token)
{
	VisStringIterator *siterator;

	siterator = visual_mem_new0 (VisStringIterator, 1);

	visual_string_iterator_init_string_token (siterator, str, token);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (siterator), TRUE);
	visual_object_ref (VISUAL_OBJECT (siterator));

	return siterator;
}

int visual_string_iterator_init_string_token (VisStringIterator *siterator, VisString *str, VisString *token)
{
	StringIteratorStringToken *ctx;

	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);
	visual_log_return_val_if_fail (token != NULL, -VISUAL_ERROR_STRING_NULL);

	visual_string_iterator_init (siterator, VISUAL_STRING_ITERATOR_TYPE_STRING, str,
			string_iterator_string_token_first,
			string_iterator_string_token_next,
			string_iterator_string_token_prev,
			string_iterator_string_token_last,
			VISUAL_OBJECT (visual_object_new_and_clean (StringIteratorStringToken)));

	ctx = visual_object_get_private (VISUAL_OBJECT (siterator));

	visual_string_init (&ctx->token);
	visual_string_copy (&ctx->token, token);

	return VISUAL_OK;
}

VisStringIterator *visual_string_iterator_new_custom (VisString *str, VisStringIteratorType type,
		VisStringIteratorFirstFunc first, VisStringIteratorNextFunc next,
		VisStringIteratorPrevFunc prev, VisStringIteratorLastFunc last, VisObject *iterpriv)
{
	VisStringIterator *siterator;

	siterator = visual_mem_new0 (VisStringIterator, 1);

	visual_string_iterator_init (siterator, type, str, first, next, prev, last, iterpriv);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (siterator), TRUE);
	visual_object_ref (VISUAL_OBJECT (siterator));

	return siterator;
}

int visual_string_iterator_init (VisStringIterator *siterator, VisStringIteratorType type, VisString *str,
		VisStringIteratorFirstFunc first, VisStringIteratorNextFunc next,
		VisStringIteratorPrevFunc prev, VisStringIteratorLastFunc last, VisObject *iterpriv)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);
	visual_log_return_val_if_fail (str != NULL, -VISUAL_ERROR_STRING_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (siterator));
	visual_object_set_dtor (VISUAL_OBJECT (siterator), string_iterator_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (siterator), FALSE);

	/* Set the VisString data */
	visual_object_clean (VISUAL_OBJECT (siterator), VisStringIterator);

	siterator->type = type;

	visual_string_init (&siterator->string);
	visual_string_copy (&siterator->string, str);

	siterator->firstfunc = first;
	siterator->nextfunc = next;
	siterator->prevfunc = prev;
	siterator->lastfunc = last;
	visual_object_set_private (VISUAL_OBJECT (siterator), iterpriv);

	return VISUAL_OK;
}

int visual_string_iterator_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);

	return siterator->nextfunc(siterator, sitoken);
}

int visual_string_iterator_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);

	return siterator->prevfunc(siterator, sitoken);
}

int visual_string_iterator_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);

	return siterator->firstfunc(siterator, sitoken);
}

int visual_string_iterator_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);

	return siterator->lastfunc(siterator, sitoken);
}

VisStringIteratorType visual_string_iterator_get_type (VisStringIterator *siterator)
{
	visual_log_return_val_if_fail (siterator != NULL, -VISUAL_ERROR_STRING_ITERATOR_NULL);

	return siterator->type;
}

VisStringIteratorToken *visual_string_iterator_token_new (VisStringIterator *siterator)
{
	VisStringIteratorToken *sitoken;

	sitoken = visual_mem_new0 (VisStringIteratorToken, 1);

	visual_string_iterator_token_init (sitoken, siterator);

	/* do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (sitoken), TRUE);
	visual_object_ref (VISUAL_OBJECT (sitoken));

	return sitoken;
}

int visual_string_iterator_token_init (VisStringIteratorToken *sitoken, VisStringIterator *siterator)
{
	visual_log_return_val_if_fail (sitoken != NULL, -VISUAL_ERROR_STRING_ITERATOR_TOKEN_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (sitoken));
	visual_object_set_dtor (VISUAL_OBJECT (sitoken), string_iterator_token_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (sitoken), FALSE);

	/* Set the VisString data */
	visual_object_clean (VISUAL_OBJECT (sitoken), VisStringIteratorToken);
	sitoken->iterator = siterator;

	if (sitoken->iterator != NULL)
		visual_object_ref (VISUAL_OBJECT (sitoken->iterator));

	return VISUAL_OK;
}

int visual_string_iterator_token_get_character (VisStringIteratorToken *sitoken, char *character)
{
	visual_log_return_val_if_fail (sitoken != NULL, -VISUAL_ERROR_STRING_ITERATOR_TOKEN_NULL);

	if (sitoken->iterator != NULL) {
		visual_log_return_val_if_fail (sitoken->iterator->type == VISUAL_STRING_ITERATOR_TYPE_CHARACTER,
				-VISUAL_ERROR_STRING_ITERATOR_INVALID_TYPE);

		*character = sitoken->token.character;
	}

	return VISUAL_OK;
}

int visual_string_iterator_token_get_string (VisStringIteratorToken *sitoken, VisString **str)
{
	visual_log_return_val_if_fail (sitoken != NULL, -VISUAL_ERROR_STRING_ITERATOR_TOKEN_NULL);

	*str = NULL;

	if (sitoken->iterator != NULL) {
		visual_log_return_val_if_fail (sitoken->iterator->type == VISUAL_STRING_ITERATOR_TYPE_STRING,
				-VISUAL_ERROR_STRING_ITERATOR_INVALID_TYPE);

		*str = &sitoken->token.string;
	}

	return VISUAL_OK;
}

int visual_string_iterator_token_get_index (VisStringIteratorToken *sitoken)
{
	visual_log_return_val_if_fail (sitoken != NULL, -VISUAL_ERROR_STRING_ITERATOR_TOKEN_NULL);

	return sitoken->index;
}

/**
 * @}
 */

