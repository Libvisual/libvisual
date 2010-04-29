/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_param.c,v 1.53 2006-09-27 22:17:36 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include "lv_log.h"
#include "lv_param.h"

static int param_container_dtor (VisObject *object);
static int param_entry_dtor (VisObject *object);

static int get_next_pcall_id (VisList *callbacks);

static int limits_to_integer (VisParamEntryLimit *limit);
static int limits_to_float (VisParamEntryLimit *limit);
static int limits_to_double (VisParamEntryLimit *limit);

static int param_container_dtor (VisObject *object)
{
	VisParamContainer *paramcontainer = VISUAL_PARAMCONTAINER (object);

	visual_collection_destroy (VISUAL_COLLECTION (&paramcontainer->entries));

	return TRUE;
}

static int param_entry_dtor (VisObject *object)
{
	VisParamEntry *param = VISUAL_PARAMENTRY (object);

	if (param->string != NULL)
		visual_mem_free (param->string);

	if (param->name != NULL)
        visual_mem_free(param->name);

	if (param->objdata != NULL)
		visual_object_unref (param->objdata);

    if (param->annotation != NULL)
        visual_mem_free(param->annotation);

	visual_palette_free_colors (&param->pal);

	visual_collection_destroy (VISUAL_COLLECTION (&param->callbacks));

	param->string = NULL;
	param->name = NULL;
	param->objdata = NULL;
    param->annotation = NULL;

	return TRUE;
}

static int get_next_pcall_id (VisList *callbacks)
{
	VisListEntry *le = NULL;
	VisParamEntryCallback *pcall;
	int found = FALSE;
	int i;

	/* Walk through all possible ids */
	for (i = 0; i < VISUAL_PARAM_CALLBACK_ID_MAX; i++) {

		found = FALSE;
		/* Check all the callbacks if the id is used */
		while ((pcall = visual_list_next (callbacks, &le)) != NULL) {

			/* Found the ID, break and get ready for the next iterate */
			if (pcall->id == i) {
				found = TRUE;

				break;
			}
		}

		/* The id has NOT been found, thus is an original, and we return this as the next id */
		if (found == FALSE)
			return i;
	}

	/* This is virtually impossible, or something very wrong is going ok, but no id seems to be left */
	return -1;
}


static int limits_to_integer (VisParamEntryLimit *limit)
{
	switch (limit->type) {
		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT:
			limit->min.integer = limit->min.floating;
			limit->max.integer = limit->max.floating;
			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE:
			limit->min.integer = limit->min.doubleflt;
			limit->max.integer = limit->max.doubleflt;
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

static int limits_to_float (VisParamEntryLimit *limit)
{
	switch (limit->type) {
		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER:
			limit->min.floating = limit->min.integer;
			limit->max.floating = limit->max.integer;
			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE:
			limit->min.floating = limit->min.doubleflt;
			limit->max.floating = limit->max.doubleflt;
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

static int limits_to_double (VisParamEntryLimit *limit)
{
	switch (limit->type) {
		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER:
			limit->min.doubleflt = limit->min.integer;
			limit->max.doubleflt = limit->max.integer;
			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT:
			limit->min.doubleflt = limit->min.floating;
			limit->max.doubleflt = limit->max.floating;
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

/**
 * @defgroup VisParam VisParam
 * @{
 */

/**
 * Creates a new VisParamContainer structure.
 *
 * @return A newly allocated VisParamContainer structure.
 */
VisParamContainer *visual_param_container_new ()
{
	VisParamContainer *paramcontainer;

	paramcontainer = visual_mem_new0 (VisParamContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (paramcontainer), TRUE, param_container_dtor);

	visual_hashmap_init (&paramcontainer->entries, visual_object_collection_destroyer);
	visual_hashmap_set_table_size (&paramcontainer->entries, 25);

	return paramcontainer;
}

/**
 * Sets the eventqueue in the VisParamContainer, so events can be emitted on param changes.
 *
 * @param paramcontainer A pointer to the VisParamContainer to which the VisEventQueue needs to be set.
 * @param eventqueue A Pointer to the VisEventQueue that is used for the events the VisParamContainer can emit.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL on failure.
 */
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue)
{
	visual_log_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

	paramcontainer->eventqueue = eventqueue;

	return VISUAL_OK;
}

/**
 * Get the pointer to the VisEventQueue the VisParamContainer is emitting events to.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which the VisEventQueue is requested.
 * 
 * @return Pointer to the VisEventQueue possibly NULL, NULL on failure.
 */
VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer)
{
	visual_log_return_val_if_fail (paramcontainer != NULL, NULL);

	return paramcontainer->eventqueue;
}

/**
 * Adds a VisParamEntry to a VisParamContainer.
 *
 * @param paramcontainer A pointer to the VisParamContainer in which the VisParamEntry is added.
 * @param param A pointer to the VisParamEntry that is added to the VisParamContainer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL, -VISUAL_ERROR_PARAM_NULL or
 *	error values returned by visual_hashmap_put_string () on failure.
 */
int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param)
{
	visual_log_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->parent = paramcontainer;

	/* On container add, we always set changed once, so vars can be synchronised in the plugin
	 * it's event loop */
	visual_param_entry_changed (param);

	return visual_hashmap_put_string (&paramcontainer->entries, param->name, param);
}

/**
 * Adds a list of VisParamEntry elements, the list is terminated by an entry of type VISUAL_PARAM_ENTRY_TYPE_END.
 * All the elements are reallocated, so this function can be used for static param lists.
 *
 * @param paramcontainer A pointer to the VisParamContainer in which the VisParamEntry elements are added.
 * @param params A pointer to the VisParamEntry elements that are added to the VisParamContainer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_container_add_many (VisParamContainer *paramcontainer, VisParamEntry *params)
{
	VisParamEntry *pnew;
	int i = 0;

	visual_log_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	visual_log_return_val_if_fail (params != NULL, -VISUAL_ERROR_PARAM_NULL);

	while (params[i].type != VISUAL_PARAM_ENTRY_TYPE_END) {
		pnew = visual_param_entry_new (visual_param_entry_get_name (&params[i]));
		visual_param_entry_set_from_param (pnew, &params[i]);

		visual_param_container_add (paramcontainer, pnew);

		i++;
	}

	return VISUAL_OK;
}

int visual_param_container_add_many_proxy (VisParamContainer *paramcontainer, VisParamEntryProxy *proxies)
{
	VisParamEntry *pnew;
	int i = 0;

	visual_log_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	visual_log_return_val_if_fail (proxies != NULL, -VISUAL_ERROR_PARAM_PROXY_NULL);

	while (proxies[i].type != VISUAL_PARAM_ENTRY_TYPE_END) {
		pnew = visual_param_entry_new (proxies[i].name);

		visual_param_entry_set_from_proxy_param (pnew, &proxies[i]);

		visual_param_container_add (paramcontainer, pnew);

		i++;
	}

	return VISUAL_OK;
}

/**
 * Removes a VisParamEntry from the VisParamContainer by giving the name of the VisParamEntry that needs
 * to be removed.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry needs to be removed.
 * @param name The name of the VisParamEntry that needs to be removed from the VisParamContainer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL, -VISUAL_ERROR_NULL
 *	or -VISUAL_ERROR_PARAM_NOT_FOUND on failure.
 */
int visual_param_container_remove (VisParamContainer *paramcontainer, char *name)
{
	VisParamEntry *param;
	int ret = VISUAL_OK;

//	visual_log_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	/* FIXME, make a better framework for this */
	if (paramcontainer == NULL) {
		return -VISUAL_ERROR_PARAM_CONTAINER_NULL;
	}

	visual_log_return_val_if_fail (name != NULL, -VISUAL_ERROR_NULL);

	if (visual_hashmap_remove_string (&paramcontainer->entries, name, TRUE) != VISUAL_OK)
		ret = -VISUAL_ERROR_PARAM_NOT_FOUND;

	return ret;
}

/**
 * Clones the source VisParamContainer into the destination VisParamContainer. When an entry with a certain name
 * already exists in the destination container, it will be overwritten with a new value.
 *
 * @param destcont A pointer to the VisParamContainer in which the VisParamEntry values are copied.
 * @param srccont A pointer to the VisParamContainer from which the VisParamEntry values are copied.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL, on failure.
 */
int visual_param_container_copy (VisParamContainer *destcont, VisParamContainer *srccont)
{
	VisCollectionIterator iter;
	VisParamEntry *destparam;
	VisParamEntry *srcparam;
	VisParamEntry *tempparam;

	visual_log_return_val_if_fail (destcont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	visual_log_return_val_if_fail (srccont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

	visual_collection_get_iterator (&iter, VISUAL_COLLECTION (&srccont->entries));

	while (visual_collection_iterator_has_more (&iter)) {
		srcparam = visual_hashmap_chain_entry_get_data (visual_collection_iterator_get_data (&iter));
		visual_collection_iterator_next (&iter);

		tempparam = visual_param_container_get (destcont, visual_param_entry_get_name (srcparam));

		/* Already exists, overwrite */
		if (tempparam != NULL) {
			visual_param_entry_set_from_param (tempparam, srcparam);

			continue;
		}

		/* Does not yet exist, create a new entry */
		destparam = visual_param_entry_new (visual_param_entry_get_name (srcparam));
		visual_param_entry_set_from_param (destparam, srcparam);

		visual_param_container_add (destcont, destparam);
	}

	visual_object_unref (VISUAL_OBJECT (&iter));

	return VISUAL_OK;
}

/**
 * Copies matching VisParamEntry elements from srccont into destcont, matching on the name.
 *
 * @param destcont A pointer to the VisParamContainer in which the VisParamEntry values are copied.
 * @param srccont A pointer to the VisParamContainer from which the VisParamEntry values are copied.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_CONTAINER_NULL, on failure.
 */
int visual_param_container_copy_match (VisParamContainer *destcont, VisParamContainer *srccont)
{
	VisCollectionIterator iter;
	VisParamEntry *destparam;
	VisParamEntry *srcparam;

	visual_log_return_val_if_fail (destcont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
	visual_log_return_val_if_fail (srccont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

	visual_collection_get_iterator (&iter, VISUAL_COLLECTION (&destcont->entries));

	while (visual_collection_iterator_has_more (&iter)) {
		destparam = visual_hashmap_chain_entry_get_data (visual_collection_iterator_get_data (&iter));
		visual_collection_iterator_next (&iter);

		srcparam = visual_param_container_get (srccont, visual_param_entry_get_name (destparam));

		/* Already exists, overwrite */
		if (srcparam != NULL) {
			visual_param_entry_set_from_param (destparam, srcparam);
		}
	}

	visual_object_unref (VISUAL_OBJECT (&iter));

	return VISUAL_OK;
}

/**
 * Retrieve a VisParamEntry from a VisParamContainer by giving the name of the VisParamEntry that is requested.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry is requested.
 * @param name The name of the VisParamEntry that is requested from the VisParamContainer.
 *
 * @return Pointer to the VisParamEntry, or NULL.
 */
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, char *name)
{
	VisParamEntry *param;

	visual_log_return_val_if_fail (paramcontainer != NULL, NULL);
	visual_log_return_val_if_fail (name != NULL, NULL);

	param = visual_hashmap_get_string (&paramcontainer->entries, name);

	return param;
}


/**
 * Creates a new VisParamEntry structure.
 *
 * @param name The name that is assigned to the VisParamEntry.
 *
 * @return A newly allocated VisParamEntry structure.
 */
VisParamEntry *visual_param_entry_new (const char *name)
{
	VisParamEntry *param;

	param = visual_mem_new0 (VisParamEntry, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (param), TRUE, param_entry_dtor);

	visual_param_entry_set_name (param, (char *)name);

	visual_collection_set_destroyer (VISUAL_COLLECTION (&param->callbacks), visual_object_collection_destroyer);

    param->type = VISUAL_PARAM_ENTRY_TYPE_NULL;

	return param;
}

/**
 * Adds a change notification callback, this shouldn't be used to get notificated within a plugin, but is for
 * things like VisUI.
 *
 * @param param Pointer to the VisParamEntry to which a change notification callback is added.
 * @param callback The notification callback, which is called on changes in the VisParamEntry.
 * @param priv A private that can be used in the callback function.
 *
 * return callback id in the form of a positive value on succes,
 *	-VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_CALLBACK_NULL or
 *	-VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY on failure.
 */
int visual_param_entry_add_callback (VisParamEntry *param, VisParamChangedCallbackFunc callback, void *priv)
{
	VisParamEntryCallback *pcall;
	int id;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (callback != NULL, -VISUAL_ERROR_PARAM_CALLBACK_NULL);

	id = get_next_pcall_id (&param->callbacks);

	visual_log_return_val_if_fail (id >= 0, -VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY);

	pcall = visual_mem_new0 (VisParamEntryCallback, 1);

	/* Do the VisObject initialization for the VisParamEntryCallback */
	visual_object_initialize (VISUAL_OBJECT (pcall), TRUE, NULL);

	pcall->id = id;
	pcall->callback = callback;
	visual_object_set_private (VISUAL_OBJECT (pcall), priv);

	visual_list_add (&param->callbacks, pcall);

	return id;
}

/**
 * Removes a change notification callback from the list of callbacks.
 *
 * @param param Pointer to the VisParamEntry from which a change notification callback is removed.
 * @param id The callback ID that was given by the visual_param_entry_add_callback method.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_remove_callback (VisParamEntry *param, int id)
{
	VisListEntry *le = NULL;
	VisParamEntryCallback *pcall;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL) {

		if (id == pcall->id) {
			visual_list_delete (&param->callbacks, &le);

			visual_object_unref (VISUAL_OBJECT (pcall));

			return VISUAL_OK;
		}
	}

	return VISUAL_OK;
}

/**
 * Notifies all the callbacks for the given VisParamEntry parameter.
 *
 * @param param Pointer to the VisParamEntry of which all the change notification
 * 	callbacks need to be called.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_notify_callbacks (VisParamEntry *param)
{
	VisListEntry *le = NULL;
	VisParamEntryCallback *pcall;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL)
		pcall->callback (param, visual_object_get_private (VISUAL_OBJECT (pcall)));

	return VISUAL_OK;
}

/**
 * Checks if the VisParamEntry it's name is the given name.
 *
 * @param param Pointer to the VisParamEntry of which we want to check the name.
 * @param name The name we want to check against.
 *
 * @return TRUE if the VisParamEntry is the one we requested, or FALSE if not.
 */
int visual_param_entry_is (VisParamEntry *param, char *name)
{
	int ret = FALSE;

	/* FIXME make a better solution */
//	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	if (param == NULL) {
		return -VISUAL_ERROR_PARAM_NULL;
	}

	if (strcmp (param->name, name) == 0)
		ret = TRUE;

	return ret;
}

/**
 * When called, emits an event in the VisParamContainer it's VisEventQueue when the VisEventQueue
 * is set.
 *
 * @param param Pointer to the VisParamEntry that is changed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_changed (VisParamEntry *param)
{
	VisEventQueue *eventqueue;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->parent == NULL)
		return VISUAL_OK;

	eventqueue = param->parent->eventqueue;

	if (eventqueue != NULL)
		visual_event_queue_add_param (eventqueue, param);

	visual_param_entry_notify_callbacks (param);

	return VISUAL_OK;
}

/**
 * Retrieves the VisParamEntryType from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the VisParamEntryType is requested.
 *
 * @return The VisParamEntryType on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
VisParamEntryType visual_param_entry_get_type (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	return param->type;
}

/**
 * Compares two parameters with each other, When they are the same, TRUE is returned, if not FALSE.
 * Keep in mind that FALSE is always returned for VISUAL_PARAM_ENTRY_TYPE_PALETTE and VISUAL_PARAM_ENTRY_TYPE_OBJECT.
 *
 * @param src1 Pointer to the first VisParamEntry for comparison.
 * @param src2 Pointer to the second VisParamEntry for comparison.
 *
 * @return TRUE if the same, FALSE if not the same,
 *	-VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_INVALID_TYPE or -VISUAL_ERROR_IMPOSSIBLE on failure.
 */
int visual_param_entry_compare (VisParamEntry *src1, VisParamEntry *src2)
{
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (src1->type != src2->type)
		return FALSE;

	switch (src1->type) {
		case VISUAL_PARAM_ENTRY_TYPE_NULL:
			return TRUE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_STRING:
			if (!strcmp (src1->string, src2->string))
				return TRUE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
			if (src1->numeric.integer == src2->numeric.integer)
				return TRUE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
			if (src1->numeric.floating == src2->numeric.floating)
				return TRUE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
			if (src1->numeric.doubleflt == src2->numeric.doubleflt)
				return TRUE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_COLOR:
			return visual_color_compare (&src1->color, &src2->color);

			break;

		case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
			return FALSE;

			break;

		case VISUAL_PARAM_ENTRY_TYPE_OBJECT:
			return FALSE;

			break;

		default:
			visual_log (VISUAL_LOG_CRITICAL, _("param type is not valid"));

			return -VISUAL_ERROR_PARAM_INVALID_TYPE;

			break;
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

int visual_param_entry_set_from_proxy_param (VisParamEntry *param, VisParamEntryProxy *proxy)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (proxy != NULL, -VISUAL_ERROR_PARAM_PROXY_NULL);

	switch (proxy->type) {
		case VISUAL_PARAM_ENTRY_TYPE_NULL:

			break;

		case VISUAL_PARAM_ENTRY_TYPE_STRING:
			visual_param_entry_set_string (param, proxy->string);
            visual_param_entry_set_string_default (param, proxy->string);
            visual_param_entry_set_annotation (param, proxy->annotation);

			break;

		case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
			visual_param_entry_set_integer (param, proxy->value);
            visual_param_entry_set_integer_default (param, proxy->value);
			visual_param_entry_limit_set_from_limit_proxy (param, &proxy->limit);
            visual_param_entry_set_annotation (param, proxy->annotation);

			break;

		case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
			visual_param_entry_set_float (param, proxy->value);
            visual_param_entry_set_float_default (param, proxy->value);
			visual_param_entry_limit_set_from_limit_proxy (param, &proxy->limit);
            visual_param_entry_set_annotation (param, proxy->annotation);

			break;

		case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
			visual_param_entry_set_double (param, proxy->value);
            visual_param_entry_set_double_default (param, proxy->value);
			visual_param_entry_limit_set_from_limit_proxy (param, &proxy->limit);
            visual_param_entry_set_annotation (param, proxy->annotation);

			break;

		case VISUAL_PARAM_ENTRY_TYPE_COLOR:
			visual_param_entry_set_color_by_color (param, &proxy->color);
            visual_param_entry_set_color_default (param, &proxy->color);
            visual_param_entry_set_annotation (param, proxy->annotation);

			break;

		default:
			visual_log (VISUAL_LOG_CRITICAL, _("param type is not valid"));

			return -VISUAL_ERROR_PARAM_INVALID_TYPE;

			break;
	}


	return VISUAL_OK;
}

/**
 * Copies the value of the src param into the param. Also sets the param to the type of which the
 * source param is.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param src Pointer to the VisParamEntry from which the value is retrieved.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_INVALID_TYPE on failure.
 */
int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_PARAM_NULL);

	switch (src->type) {
		case VISUAL_PARAM_ENTRY_TYPE_NULL:

			break;

		case VISUAL_PARAM_ENTRY_TYPE_STRING:
			visual_param_entry_set_string (param, visual_param_entry_get_string (src));
            visual_param_entry_set_string_default (param, visual_param_entry_get_string_default (src));
            visual_param_entry_set_annotation(param, visual_param_entry_get_annotation (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
			visual_param_entry_set_integer (param, visual_param_entry_get_integer (src));
            visual_param_entry_set_integer_default (param, visual_param_entry_get_integer_default (src));
            visual_param_entry_set_annotation(param, visual_param_entry_get_annotation (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
			visual_param_entry_set_float (param, visual_param_entry_get_float (src));
            visual_param_entry_set_float_default (param, visual_param_entry_get_float_default (src));
            visual_param_entry_set_annotation(param, visual_param_entry_get_annotation (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
			visual_param_entry_set_double (param, visual_param_entry_get_double (src));
            visual_param_entry_set_double_default (param, visual_param_entry_get_double_default (src));
            visual_param_entry_set_annotation(param, visual_param_entry_get_annotation (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_COLOR:
			visual_param_entry_set_color_by_color (param, visual_param_entry_get_color (src));
            visual_param_entry_set_color_default (param, visual_param_entry_get_color_default (src));
            visual_param_entry_set_annotation(param, visual_param_entry_get_annotation (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
			visual_param_entry_set_palette (param, visual_param_entry_get_palette (src));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_OBJECT:
			visual_param_entry_set_object (param, visual_param_entry_get_object (src));

			break;

		default:
			visual_log (VISUAL_LOG_CRITICAL, _("param type is not valid"));

			return -VISUAL_ERROR_PARAM_INVALID_TYPE;

			break;
	}


	return VISUAL_OK;
}

/**
 * Set the name for a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry to which the name is set.
 * @param name The name that is set to the VisParamEntry.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_name (VisParamEntry *param, char *name)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if(param->name != NULL)
        visual_mem_free(param->name);

    param->name = strdup(name);

	return VISUAL_OK;
}

/**
 * Calls visual_param_entry_set_string_no_event and flags the entry as changed if needed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param string The string for this parameter.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_string (VisParamEntry *param, char *string)
{
    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    int ret = visual_param_entry_set_string_no_event(param, string);

    if(ret == VISUAL_PARAM_CHANGED) {
        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_STRING and assigns the string given as argument to it.
 * This does not flag the VisParamEntry as changed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param string The string for this parameter.
 *
 * @return VISUAL_OK if nothing changed, VISUAL_PARAM_CHANGED if the entry changed,
 * or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_string_no_event (VisParamEntry *param, char *string)
{
    int ret = VISUAL_PARAM_CHANGED;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_STRING;

	if (string == NULL && param->string == NULL)
		return ret;

	if (string == NULL && param->string != NULL) {
		visual_mem_free (param->string);
		param->string = NULL;

	} else if (param->string == NULL && string != NULL) {
		param->string = strdup (string);

	} else if (strcmp (string, param->string) != 0) {
		visual_mem_free (param->string);

		param->string = strdup (string);
	} else {
        ret = VISUAL_OK;
    }

	return ret;
}

/**
 * Sets the VisParamEntry's default string data.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param string The default string for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_string_default(VisParamEntry *param, char *string)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if(string == NULL && param->string_default == NULL)
        return VISUAL_OK;

    if(string == NULL && param->string_default != NULL) {
        visual_mem_free (param->string_default);
        param->string_default = NULL;

    } else if (param->string_default == NULL && string != NULL) {
        param->string_default = strdup(string);
        
    } else if (strcmp (string, param->string_default) != 0) {
        visual_mem_free(param->string_default);

        param->string_default = strdup (string);
    }

    return VISUAL_OK;
}

/**
 * This calls visual_param_entry_set_integer_no_event and flags the entry changed if needed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param integer The integer value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_integer (VisParamEntry *param, int integer)
{
    int ret;

    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    ret = visual_param_entry_set_integer_no_event(param, integer);

    if( ret == VISUAL_PARAM_CHANGED ) {
        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_INTEGER and assigns the integer given as argument to it.
 * This does not flag the entry changed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param integer The integer value for this parameter.
 *
 * @return VISUAL_OK if nothing changed, VISUAL_PARAM_CHANGED if the param changed,
 * or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_integer_no_event (VisParamEntry *param, int integer)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		limits_to_integer (&param->limit);

	param->type = VISUAL_PARAM_ENTRY_TYPE_INTEGER;

	visual_param_entry_limit_nearest_integer (param, &integer);

	if (param->numeric.integer != integer) {
		param->numeric.integer = integer;

        return VISUAL_PARAM_CHANGED;
	}

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry's default integer value.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param integer The default integer value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_integer_default( VisParamEntry *param, int integer)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->numeric_default.integer = integer;

    return VISUAL_OK;
}

/**
 * This calls visual_param_entry_set_float_no_event and flags the entry changed if needed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The float value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_float (VisParamEntry *param, float floating)
{
    int ret;

    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
    
    ret = visual_param_entry_set_float_no_event(param, floating);

    if (ret == VISUAL_PARAM_CHANGED) {
        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_FLOAT and assigns the float given as argument to it.
 * This does not flag the entry as changed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The float value for this parameter.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_float_no_event (VisParamEntry *param, float floating)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		limits_to_float (&param->limit);

	param->type = VISUAL_PARAM_ENTRY_TYPE_FLOAT;

	visual_param_entry_limit_nearest_float (param, &floating);

	if (param->numeric.floating != floating) {
		param->numeric.floating = floating;

		visual_param_entry_changed (param);
        return VISUAL_PARAM_CHANGED;
	}

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry's default float value.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The default float value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_float_default (VisParamEntry *param, float floating)
{
    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->numeric_default.floating = floating;

    return VISUAL_OK;
}

/**
 * This calls visual_param_entry_set_double_no_event and flags the param changed if needed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The double value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt)
{
    int ret;

    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    ret = visual_param_entry_set_double_no_event (param, doubleflt);

    if(ret == VISUAL_PARAM_CHANGED) {
        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_DOUBLE and assigns the double given as argument to it.
 * This does not flag the param changed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param doubleflt The double value for this parameter.
 *
 * @return VISUAL_OK on succes, VISUAL_PARAM_CHANGED if the entry changed,
 * or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_double_no_event (VisParamEntry *param, double doubleflt)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		limits_to_double (&param->limit);

	param->type = VISUAL_PARAM_ENTRY_TYPE_DOUBLE;

	visual_param_entry_limit_nearest_double (param, &doubleflt);

	if (param->numeric.doubleflt != doubleflt) {
		param->numeric.doubleflt = doubleflt;

        return VISUAL_PARAM_CHANGED;
	}

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry's default double value.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param doubleflt The default double value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_double_default (VisParamEntry *param, double doubleflt)
{
    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->numeric_default.doubleflt = doubleflt;

    return VISUAL_OK;
}


/**
 * This calls visual_param_entry_set_color_no_event and flags the param changed if needed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param r The red value for this color parameter.
 * @param g The green value for this color parameter.
 * @param b The blue value for this color parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b)
{
    int ret;
    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    ret = visual_param_entry_set_color_no_event (param, r, g, b);

    if (ret == VISUAL_PARAM_CHANGED) {
        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLOR and assigns the rgb values.
 * This does not flag the entry as changed.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param r The red value for this color parameter.
 * @param g The green value for this color parameter.
 * @param b The blue value for this color parameter.
 *
 * @return VISUAL_OK on succes, VISUAL_PARAM_CHANGED if the entry changed,
 * or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color_no_event (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_COLOR;

	if (param->color.r != r || param->color.g != g || param->color.b != b) {
		visual_color_set (&param->color, r, g, b);
        
        return VISUAL_PARAM_CHANGED;
	}

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLOR and assigns the rgb values from the given VisColor as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param color Pointer to the VisColor from which the rgb values are copied into the parameter.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color_by_color (VisParamEntry *param, VisColor *color)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_COLOR;

	if (visual_color_compare (&param->color, color) == FALSE) {
		visual_color_copy (&param->color, color);

		visual_param_entry_changed (param);
	}

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry's default VisColor.
 *
 * @param param Pointer to the VisParamEntry to which the parameter is set.
 * @param color Pointer to the VisColor from which the rgb values are copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color_default (VisParamEntry *param, VisColor *color)
{
    visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    visual_color_copy(&param->color_default, color);

    return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_PALETTE and assigns a VisPalette to the VisParamEntry.
 * This function does not check if there is a difference between the prior set palette and the new one, and always
 * emits the changed event. so watch out with usage.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param pal Pointer to the VisPalette from which the palette data is retrieved for the VisParamEntry.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_palette (VisParamEntry *param, VisPalette *pal)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_PALETTE;

	visual_palette_free_colors (&param->pal);

	if (pal != NULL) {
		visual_palette_allocate_colors (&param->pal, pal->ncolors);

		visual_palette_copy (&param->pal, pal);
	}

	visual_param_entry_changed (param);

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLLECTION and assigns a VisCollection to the VisParamEntry.
 * With a VisCollection VisParamEntry, the VisCollection is referenced, not cloned.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param collection Pointer to the VisCollection that is linked to the VisParamEntry.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_collection (VisParamEntry *param, VisCollection *collection)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_COLLECTION;

	if (param->collection != NULL)
		visual_object_unref (VISUAL_OBJECT(param->collection));

	param->collection = collection;

	if (param->collection != NULL)
		visual_object_ref (VISUAL_OBJECT(param->collection));

	visual_param_entry_changed (param);

	return VISUAL_OK;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_OBJECT and assigns a VisObject to the VisParamEntry.
 * With a VisObject VisParamEntry, the VisObject is referenced, not cloned.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param object Pointer to the VisObject that is linked to the VisParamEntry.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_object (VisParamEntry *param, VisObject *object)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->type = VISUAL_PARAM_ENTRY_TYPE_OBJECT;

	if (param->objdata != NULL)
		visual_object_unref (param->objdata);

	param->objdata = object;

	if (param->objdata != NULL)
		visual_object_ref (param->objdata);

	visual_param_entry_changed (param);

	return VISUAL_OK;
}

/**
 * Set the annotation for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry.
 * @param ann The annotation to be set.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_annotation(VisParamEntry *param, char *ann)
{
    visual_log_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if(ann == NULL)
        return -VISUAL_ERROR_GENERAL;

    if(param->annotation != NULL)
        visual_mem_free(param->annotation);

    param->annotation = strdup(ann);

    return VISUAL_OK;
}

/**
 * Get the name of the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the name is requested.
 *
 * @return The name of the VisParamEntry or NULL.
 */
char *visual_param_entry_get_name (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	return param->name;
}

/**
 * Get the string parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the string parameter is requested.
 *
 * @return The string parameter from the VisParamEntry or NULL.
 */
char *visual_param_entry_get_string (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_STRING) {
		visual_log (VISUAL_LOG_WARNING, _("Requesting string from a non string param"));

		return NULL;
	}

	return param->string;
}

/**
 * Get the default string parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry to which the default string parameter is requested.
 *
 * @return The default string parameter from the VisParamEntry or NULL.
 */
char *visual_param_entry_get_string_default (VisParamEntry *param)
{
    visual_log_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_STRING) {
        visual_log (VISUAL_LOG_WARNING, _("Requesting default string from non string param"));

        return NULL;
    }

    return param->string_default;
}

/**
 * Get the integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the integer parameter is requested.
 *
 * @return The integer parameter from the VisParamEntry.
 */
int visual_param_entry_get_integer (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, 0);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_INTEGER)
		visual_log (VISUAL_LOG_WARNING, _("Requesting integer from a non integer param"));

	return param->numeric.integer;
}

/**
 * Get the default integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the default integer parameter is requested.
 *
 * @return The integer parameter from the VisParamEntry.
 */
int visual_param_entry_get_integer_default (VisParamEntry *param)
{
    visual_log_return_val_if_fail (param != NULL, 0);

    if(param->type != VISUAL_PARAM_ENTRY_TYPE_INTEGER)
        visual_log (VISUAL_LOG_WARNING, _("Requesting default integer from a non integer param"));

    return param->numeric_default.integer;
}

/**
 * Get the float parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the float parameter is requested.
 *
 * @return The float parameter from the VisParamEntry.
 */
float visual_param_entry_get_float (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, 0);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_FLOAT)
		visual_log (VISUAL_LOG_WARNING, _("Requesting float from a non float param"));

	return param->numeric.floating;
}

/**
 * Get the default float parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the default float parameter is requested.
 *
 * @return The default float parameter from the VisParamEntry.
 */
float visual_param_entry_get_float_default (VisParamEntry *param)
{
    visual_log_return_val_if_fail (param != NULL, 0);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_FLOAT)
        visual_log (VISUAL_LOG_WARNING, _("Requesting default float from a non float param"));

    return param->numeric_default.floating;
}

/**
 * Get the double parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the double parameter is requested.
 *
 * @return The double parameter from the VisParamEntry.
 */
double visual_param_entry_get_double (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, 0);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
		visual_log (VISUAL_LOG_WARNING, _("Requesting double from a non double param"));

	return param->numeric.doubleflt;
}

/**
 * Get the default double parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the default double parameter is requested.
 *
 * @return The default double parameter from the VisParamEntry.
 */
double visual_param_entry_get_double_default (VisParamEntry *param)
{
    visual_log_return_val_if_fail (param != NULL, 0);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
        visual_log (VISUAL_LOG_WARNING, _("Requesting default double from a non double param"));

    return param->numeric_default.doubleflt;
}

/**
 * Get the color parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the color parameter is requested.
 *
 * @return Pointer to the VisColor parameter from the VisParamEntry. It's adviced to
 *	use the VisColor that is returned as read only seen changing it directly won't emit events and
 *	can cause synchronous problems between the plugin and the parameter system. Instead use the
 *	visual_param_entry_set_color* methods to change the parameter value.
 */
VisColor *visual_param_entry_get_color (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
		visual_log (VISUAL_LOG_WARNING, _("Requesting color from a non color param"));

		return NULL;
	}

	return &param->color;
}

/**
 * Get the default color parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the default color parameter is requested.
 *
 * @return Pointer to the default VisColor parameter from the VisParamEntry. 
 */
VisColor *visual_param_entry_get_color_default (VisParamEntry *param)
{
    visual_log_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
        visual_log (VISUAL_LOG_WARNING, _("Requesting default color from a non color param"));

        return NULL;
    }

    return &param->color_default;
}


/**
 * Get the palette parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the palette parameter is requested.
 *
 * @return Pointer to the VisPalette parameter from the VisParamEntry. The returned VisPalette
 *	should be exclusively used as read only.
 */
VisPalette *visual_param_entry_get_palette (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_PALETTE) {
		visual_log (VISUAL_LOG_WARNING, _("Requested palette from a non palette param\n"));

		return NULL;
	}

	return &param->pal;
}

/**
 * Get the object parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the object parameter is requested.
 *
 * @return Pointer to the VisObject parameter from the VisParamEntry.
 */
VisObject *visual_param_entry_get_object (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_OBJECT) {
		visual_log (VISUAL_LOG_WARNING, _("Requested object from a non object param\n"));

		return NULL;
	}

	return param->objdata;
}


/**
 * Get the collection parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the collection parameter is requested.
 *
 * @return Pointer to the VisCollection parameter from the VisParamEntry.
 */
VisCollection *visual_param_entry_get_collection (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, NULL);

	if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLLECTION) {
		visual_log (VISUAL_LOG_WARNING, _("Requested collection from non collection param\n"));

		return NULL;
	}

	return param->collection;
}

/**
 * Get the annotation parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the annotation parameter is requested.
 *
 * @return Pointer to the annotation parameter from the VisParamEntry.
 */
char *visual_param_entry_get_annotation(VisParamEntry *param)
{
    visual_log_return_val_if_fail(param != NULL, NULL);

    return param->annotation;
}

int visual_param_entry_limit_set_from_limit_proxy (VisParamEntry *param, VisParamEntryLimitProxy *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (limit != NULL, -VISUAL_ERROR_PARAM_LIMIT_PROXY_NULL);

	rlimit.type = limit->type;

	switch (limit->type) {
		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER:
			rlimit.min.integer = limit->min.integer;
			rlimit.max.integer = limit->max.integer;

			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT:
			rlimit.min.floating = limit->min.floating;
			rlimit.max.floating = limit->max.floating;

			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE:
			rlimit.min.doubleflt = limit->min.doubleflt;
			rlimit.max.doubleflt = limit->max.doubleflt;

			break;

		case VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL:
			break;

		default:
			return -VISUAL_ERROR_PARAM_INVALID_TYPE;

			break;
	}

	return visual_param_entry_limit_set_from_limit (param, &rlimit);
}

int visual_param_entry_limit_set_from_limit (VisParamEntry *param, VisParamEntryLimit *limit)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
	visual_log_return_val_if_fail (limit != NULL, -VISUAL_ERROR_PARAM_LIMIT_NULL);

	if (limit->type == VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL) {
		param->limit.type = VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL;

		return VISUAL_OK;
	}

	switch (param->type) {
		case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
			visual_param_entry_limit_set_integer (param,
					visual_param_entry_limit_get_integer_minimum (limit),
					visual_param_entry_limit_get_integer_maximum (limit));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
			visual_param_entry_limit_set_float (param,
					visual_param_entry_limit_get_float_minimum (limit),
					visual_param_entry_limit_get_float_maximum (limit));

			break;

		case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
			visual_param_entry_limit_set_double (param,
					visual_param_entry_limit_get_double_minimum (limit),
					visual_param_entry_limit_get_double_maximum (limit));
			break;

		default:
			return -VISUAL_ERROR_PARAM_INVALID_TYPE;

			break;
	}

	return VISUAL_OK;
}

int visual_param_entry_limit_set_integer (VisParamEntry *param, int min, int max)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->limit.type = VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER;
	param->limit.min.integer = min;
	param->limit.max.integer = max;

	return VISUAL_OK;
}

int visual_param_entry_limit_set_float (VisParamEntry *param, float min, float max)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->limit.type = VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT;
	param->limit.min.floating = min;
	param->limit.max.floating = max;

	return VISUAL_OK;
}

int visual_param_entry_limit_set_double (VisParamEntry *param, double min, double max)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	param->limit.type = VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE;
	param->limit.min.doubleflt = min;
	param->limit.max.doubleflt = max;

	return VISUAL_OK;
}

int visual_param_entry_limit_nearest_integer (VisParamEntry *param, int *integer)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->limit.type == VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL)
		return VISUAL_OK;

	if (param->limit.min.integer > *integer) {
		*integer = param->limit.min.integer;

		return TRUE;
	} else if (param->limit.max.integer < *integer) {
		*integer = param->limit.max.integer;

		return TRUE;
	}

	return VISUAL_OK;
}

int visual_param_entry_limit_nearest_float (VisParamEntry *param, float *floating)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->limit.type == VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL)
		return VISUAL_OK;

	if (param->limit.min.floating > *floating) {
		*floating = param->limit.min.floating;

		return TRUE;
	} else if (param->limit.max.floating < *floating) {
		*floating = param->limit.max.floating;

		return TRUE;
	}

	return VISUAL_OK;
}

int visual_param_entry_limit_nearest_double (VisParamEntry *param, double *doubleflt)
{
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	if (param->limit.type == VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL)
		return VISUAL_OK;

	if (param->limit.min.doubleflt > *doubleflt) {
		*doubleflt = param->limit.min.doubleflt;

		return TRUE;
	} else if (param->limit.max.doubleflt < *doubleflt) {
		*doubleflt = param->limit.max.doubleflt;

		return TRUE;
	}

	return VISUAL_OK;
}

int visual_param_entry_limit_get_integer_minimum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_integer (&rlimit);

	return rlimit.min.integer;
}

int visual_param_entry_limit_get_integer_maximum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_integer (&rlimit);

	return rlimit.max.integer;
}

float visual_param_entry_limit_get_float_minimum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0.0f);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_float (&rlimit);

	return rlimit.min.floating;
}

float visual_param_entry_limit_get_float_maximum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0.0f);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_float (&rlimit);

	return rlimit.max.floating;
}

double visual_param_entry_limit_get_double_minimum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0.0f);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_double (&rlimit);

	return rlimit.min.doubleflt;
}

double visual_param_entry_limit_get_double_maximum (VisParamEntryLimit *limit)
{
	VisParamEntryLimit rlimit;

	visual_log_return_val_if_fail (limit != NULL, 0.0f);

	visual_mem_copy (&rlimit, limit, sizeof (VisParamEntryLimit));

	limits_to_double (&rlimit);

	return rlimit.max.doubleflt;
}

/**
 * @}
 */

