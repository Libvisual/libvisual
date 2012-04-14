/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * List implementation from RCL.
 * Copyright (C) 2002, 2003, 2004
 *				Dennis Smit <ds@nerds-incorporated.org>,
 *				Sepp Wijnands <mrrazz@nerds-incorporated.org>,
 *				Tom Wimmenhove <nohup@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Sepp Wijnands <mrrazz@nerds-incorporated.org>,
 *	    Tom Wimmenhove <nohup@nerds-incorporated.org>
 *
 * $Id: lv_list.h,v 1.19 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_LIST_H
#define _LV_LIST_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_collection.h>

/**
 * @defgroup VisList VisList
 * @{
 */

#define VISUAL_LIST(obj)				(VISUAL_CHECK_CAST ((obj), VisList))

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
 * The VisList data structure represents a linked list. It inherents from the
 * VisCollection class.
 */
struct _VisList {
	VisCollection		 collection;	/**< The VisCollection data. */

	VisListEntry		*head;		/**< Pointer to the beginning of the list. */
	VisListEntry		*tail;		/**< Pointer to the end of the list. */

	int			 count;		/**< Number of entries that are in the list. */
};

LV_BEGIN_DECLS

/**
 * Creates a new VisList structure.
 * The VisList system is a double linked list implementation.
 *
 * @return A newly allocated VisList.
 */
LV_API VisList *visual_list_new (VisCollectionDestroyerFunc destroyer);

/**
 * Initializes a new VisList
 *
 * @p list - newly generated list (output of @ref visual_list_new() )
 * @p destroyer - the function that cleans up the list upon @ref visual_list_destroy()
 */
LV_API int visual_list_init (VisList *list, VisCollectionDestroyerFunc destroyer);

/**
 * Goes to the next entry in the list and return it's data element.
 * This function will load the next entry in le and return a pointer
 * to the data element.
 *
 * @see visual_list_prev
 *
 * @param list Pointer to the VisList we're traversing.
 * @param le Pointer to a VisListEntry to store the next entry within
 * 	and also to use as a reference to determine at which entry we're
 * 	currently. To begin traversing do: VisListEntry *le = NULL and pass
 * 	it as &le in the argument.
 *
 * @return The data element of the next entry, or NULL.
 */
LV_API void *visual_list_next (VisList *list, VisListEntry **le);

/**
 * Go to the previous entry in the list and return it's data element.
 * This function will load the previous entry in le and return a pointer
 * to the data element.
 *
 * @see visual_list_next
 *
 * @param list Pointer to the VisList we're traversing.
 * @param le Pointer to a VisListEntry to store the previous entry within
 * 	and also to use as a reference to determine at which entry we're
 * 	currently. To begin traversing at the end of the list do:
 * 	VisList *le = NULL and pass it as &le in the argument.
 *
 * @return The data element of the previous entry, or NULL.
 */
LV_API void *visual_list_prev (VisList *list, VisListEntry **le);

/**
 * Get an data entry by index. This will give the pointer to an data
 * element based on the index in the list.
 *
 * @param list Pointer to the VisList of which we want an element.
 * @param index Index to determine which entry we want. The index starts at
 * 	1.
 *
 * @return The data element of the requested entry, or NULL.
 */
LV_API void *visual_list_get (VisList *list, int index);

/**
 * Get an data entry by index. This will give the pointer to an data
 * element based on the index in the list.
 *
 * @param list Pointer to the VisList of which we want an element.
 * @param index Index to determine which entry we want. The index starts at
 * 	1.
 *
 * @return The data element of the requested entry, or NULL.
 */
LV_API int visual_list_add_at_begin (VisList *list, void *data);

/**
 * Adds an entry at the end of the list.
 *
 * @param list Pointer to the VisList to which an entry needs to be added
 * 	at it's tail.
 * @param data A pointer to the data that needs to be added to the list.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL on failure.
 */
LV_API int visual_list_add (VisList *list, void *data);

/**
 * Chains an VisListEntry at the beginning of the list.
 *
 * @param list Pointer to the VisList to which an entry needs to be added
 * 	at it's tail.
 * @param le A pointer to the VisListEntry that needs to be chained to the list.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL or -VISUAL_ERROR_LIST_ENTRY_NULL on failure.
 */
LV_API int visual_list_chain_at_begin (VisList *list, VisListEntry *le);

/**
 * Chains an VisListEntry at the end of the list.
 *
 * @param list Pointer to the VisList to which an entry needs to be added
 * 	at it's tail.
 * @param le A pointer to the VisListEntry that needs to be chained to the list.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL or -VISUAL_ERROR_LIST_ENTRY_NULL on failure.
 */
LV_API int visual_list_chain (VisList *list, VisListEntry *le);

/**
 * Unchain a VisListEntry from a VisList, entry won't be deleted. This function will only remove the
 * links with it's VisList.
 *
 * @param list Pointer to the VisList from which an entry is unchained.
 * @param le Pointer to a VisListEntry that is being unchained.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL or -VISUAL_ERROR_LIST_ENTRY_NULL
 * 	on failure.
 */
LV_API int visual_list_unchain (VisList *list, VisListEntry *le);

/**
 * Insert an entry in the middle of a list. By adding it
 * after the le entry.
 *
 * @param list Pointer to the VisList in which an entry needs to be inserted.
 * @param le Pointer to a VisListEntry after which the entry needs to be inserted.
 * @param data Pointer to the data the new entry represents.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL, -VISUAL_ERROR_LIST_ENTRY_NULL or
 * 	-VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_list_insert (VisList *list, VisListEntry **le, void *data);

/**
 * Removes an entry from the list.
 *
 * @param list A pointer to the VisList in which an entry needs to be deleted.
 * @param le A pointer to the entry that needs to be deleted.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL or -VISUAL_ERROR_LIST_ENTRY_NULL on failure.
 */
LV_API int visual_list_delete (VisList *list, VisListEntry **le);

/**
 * Removes and entry from the list and uses the VisListDestroyerFunc when present to clean up the data.
 *
 * @param list A pointer to the VisList in which an entry needs to be destroyed.
 * @param le A pointer to the entry that needs to be destroyed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_LIST_NULL or -VISUAL_ERROR_LIST_ENTRY_NULL on failure.
 */
LV_API int visual_list_destroy (VisList *list, VisListEntry **le);

/**
 * Gets the amount of elements in a VisList
 *
 * @p list - the list of which the element-count is requested
 * @return amount of elements currently in list (or -VISUAL_ERROR_COLLECTION_NULL on error)
 */
LV_API int visual_list_count (VisList *list);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_LIST_H */
