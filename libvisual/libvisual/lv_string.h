/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_string.h,v 1.1 2006-09-27 22:19:58 synap Exp $
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

#ifndef _LV_STRING_H
#define _LV_STRING_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_buffer.h>
#include <libvisual/lv_list.h>

VISUAL_BEGIN_DECLS

#define VISUAL_STRING(obj)				(VISUAL_CHECK_CAST ((obj), VisString))
#define VISUAL_STRING_ITERATOR(obj)			(VISUAL_CHECK_CAST ((obj), VisStringIterator))
#define VISUAL_STRING_ITERATOR_TOKEN(obj)		(VISUAL_CHECK_CAST ((obj), VisStringIteratorToken))

/**
 * Build string parameter macro. Builds a string from a constant string expression purely
 * for function parameter usage.
 */
#define VIS_BSTR(x)	(visual_string_new_parameter_string (x))
#define VIS_CSTR(x)	(visual_string_get_cstring (x))
#define VIS_CSTR_DUP(x)	(strdup (visual_string_get_cstring (x)))

typedef enum {
	VISUAL_STRING_ITERATOR_TYPE_NONE	= 0,
	VISUAL_STRING_ITERATOR_TYPE_CHARACTER	= 1,
	VISUAL_STRING_ITERATOR_TYPE_STRING	= 2
} VisStringIteratorType;

typedef enum {
	VISUAL_STRING_FLAG_NONE			= 0,
	VISUAL_STRING_FLAG_FORCE_COPY		= 1,
	VISUAL_STRING_FLAG_READONLY		= 2,
	VISUAL_STRING_FLAG_PARAMETER		= 4,
	VISUAL_STRING_FLAG_IN_POOL		= 8
} VisStringFlags;

typedef struct _VisString VisString;
typedef struct _VisStringIterator VisStringIterator;
typedef struct _VisStringIteratorToken VisStringIteratorToken;


typedef int (*VisStringIteratorFirstFunc)(VisStringIterator *siterator, VisStringIteratorToken **sitoken);
typedef int (*VisStringIteratorNextFunc)(VisStringIterator *siterator, VisStringIteratorToken **sitoken);
typedef int (*VisStringIteratorPrevFunc)(VisStringIterator *siterator, VisStringIteratorToken **sitoken);
typedef int (*VisStringIteratorLastFunc)(VisStringIterator *siterator, VisStringIteratorToken **sitoken);


/**
 * The VisString data structure holds a character string.
 */
struct _VisString {
	VisObject		 object;	/**< The VisObject data. */

	VisString		*parent;	/**< The parent VisString if any. */
	VisListEntry		*parententry;	/**< The VisList entry of this child
						 * in relation to the parent. */

	VisBuffer		*buffer;	/**< The storage backing VisBuffer. */

	VisList			*childs;	/**< Childs that need on-change copy when
						 * the parent is modified. */

	int			 flags;		/**< Flags that define the string behavior. */

	int			 length;	/**< Length of string. */
};

/**
 * The VisStringIterator structure holds string iterating data.
 */
struct _VisStringIterator {
	VisObject			 object;

	VisString			 string;

	VisStringIteratorType		 type;

	VisStringIteratorFirstFunc	 firstfunc;
	VisStringIteratorNextFunc	 nextfunc;
	VisStringIteratorPrevFunc	 prevfunc;
	VisStringIteratorLastFunc	 lastfunc;
};

/**
 * The VisStringIteratorToken structure holds a iterator token.
 */
struct _VisStringIteratorToken {
	VisObject		 object;

	VisStringIterator	*iterator;

	union {
		char		 character;
		VisString	 string;
	} token;

	int			 index;
};


/* prototypes */
int visual_string_initialize (void);
int visual_string_is_initialized (void);
int visual_string_deinitialize (void);

VisString *visual_string_new (void);
VisString *visual_string_new_with_value (const char *s);
VisString *visual_string_new_copy (VisString *src);
VisString *visual_string_new_parameter_string (const char *s);

int visual_string_init (VisString *str);
int visual_string_init_force_copy (VisString *str, int force_copy);

int visual_string_ref_parameter (VisString *str);
int visual_string_unref_parameter (VisString *str);

const char *visual_string_get_cstring (VisString *str);
VisBuffer *visual_string_get_buffer (VisString *str);

int visual_string_force_duplication (VisString *str, int force_copy);

int visual_string_copy (VisString *dest, VisString *src);
int visual_string_n_copy (VisString *dest, VisString *src, visual_size_t n);

int visual_string_copy_value (char *dest, VisString *src);
int visual_string_n_copy_value (char *dest, VisString *src, visual_size_t n);
int visual_string_n_copy_value_offset (char *dest, VisString *src, int offset, visual_size_t n);

int visual_string_length (VisString *str);

int visual_string_substring (VisString *dest, VisString *src, int begin, int end);
int visual_string_substring_begin (VisString *dest, VisString *src, int begin);
int visual_string_substring_end (VisString *dest, VisString *src, int end);

VisString *visual_string_set (VisString *str, const char *s);
int visual_string_set_many (VisString *str, const char *s, ...);

int visual_string_combine (VisString *dest, VisString *src);
int visual_string_remove (VisString *str, int begin, int end);
int visual_string_fill (VisString *str, char c, visual_size_t n);

int visual_string_compare (VisString *str1, VisString *str2);
int visual_string_n_compare (VisString *str1, VisString *str2, visual_size_t n);

int visual_string_case_compare (VisString *str1, VisString *str2);
int visual_string_n_case_compare (VisString *str1, VisString *str2, visual_size_t n);

int visual_string_ends_with (VisString *str1, VisString *str2);
int visual_string_starts_with (VisString *str1, VisString *str2);
int visual_string_starts_with_offset (VisString *str1, VisString *str2, int offset);

int visual_string_find_char (VisString *str, char c);
int visual_string_find_char_offset (VisString *str, char c, int offset);
int visual_string_reverse_find_char (VisString *str, char c);

int visual_string_find_char_set (VisString *str, VisString *set);
int visual_string_find_char_set_offset (VisString *str, VisString *set, int offset);
int visual_string_reverse_find_char_set (VisString *str, VisString *set);

int visual_string_find_string (VisString *haystack, VisString *needle);
int visual_string_find_string_offset (VisString *haystack, VisString *needle, int offset);

int visual_string_find_last_string (VisString *haystack, VisString *needle);
int visual_string_find_last_string_offset (VisString *haystack, VisString *needle, int offset);

int visual_string_replace (VisString *str, char oldchar, char newchar);
int visual_string_replace_offset (VisString *str, char oldchar, char newchar, int offset);
int visual_string_replace_string (VisString *str, VisString *oldstr, VisString *newstr);
int visual_string_replace_string_offset (VisString *str, VisString *oldstr, VisString *newstr, int offset);

int visual_string_to_lower_case (VisString *str);
int visual_string_to_upper_case (VisString *str);

int visual_string_trim (VisString *str);

int visual_string_count_char_sequencial (VisString *str, char c, int offset);
int visual_string_count_char_sequencial_tail (VisString *str, char c, int offset);

uint32_t visual_string_get_hashcode (VisString *str);
uint32_t visual_string_get_hashcode_cstring (const char *s);

VisStringIterator *visual_string_iterator_new (VisString *str);

VisStringIterator *visual_string_iterator_new_all (VisString *str);
int visual_string_iterator_init_all (VisStringIterator *siterator, VisString *str);

VisStringIterator *visual_string_iterator_new_token (VisString *str, char token);
int visual_string_iterator_init_token (VisStringIterator *siterator, VisString *str, char token);

VisStringIterator *visual_string_iterator_new_token_set (VisString *str, VisString *tokenset);
int visual_string_iterator_init_token_set (VisStringIterator *siterator, VisString *str, VisString *tokenset);

VisStringIterator *visual_string_iterator_new_string_token (VisString *str, VisString *token);
int visual_string_iterator_init_string_token (VisStringIterator *siterator, VisString *str, VisString *token);

VisStringIterator *visual_string_iterator_new_custom (VisString *str, VisStringIteratorType type,
		VisStringIteratorFirstFunc first, VisStringIteratorNextFunc next,
		VisStringIteratorPrevFunc prev, VisStringIteratorLastFunc last, VisObject *iterpriv);

int visual_string_iterator_init (VisStringIterator *siterator, VisStringIteratorType type, VisString *str,
		VisStringIteratorFirstFunc first, VisStringIteratorNextFunc next,
		VisStringIteratorPrevFunc prev, VisStringIteratorLastFunc last, VisObject *iterpriv);

int visual_string_iterator_next (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
int visual_string_iterator_prev (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
int visual_string_iterator_first (VisStringIterator *siterator, VisStringIteratorToken **sitoken);
int visual_string_iterator_last (VisStringIterator *siterator, VisStringIteratorToken **sitoken);

VisStringIteratorType visual_string_iterator_get_type (VisStringIterator *siterator);

VisStringIteratorToken *visual_string_iterator_token_new (VisStringIterator *siterator);
int visual_string_iterator_token_init (VisStringIteratorToken *sitoken, VisStringIterator *siterator);

int visual_string_iterator_token_get_character (VisStringIteratorToken *sitoken, char *character);
int visual_string_iterator_token_get_string (VisStringIteratorToken *sitoken, VisString **str);

int visual_string_iterator_token_get_index (VisStringIteratorToken *sitoken);

VISUAL_END_DECLS

#endif /* _LV_STRING_H */
