#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_log.h"
#include "lv_param.h"

static void param_list_destroy (void *data);

static void param_list_destroy (void *data)
{
	VisParamEntry *param;

	if (data == NULL)
		return;

	param = (VisParamEntry *) data;

	visual_param_entry_free (param);
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
	
	return paramcontainer;
}

/**
 * Destroys a VisParamContainer and all it's VisParamEntry members.
 *
 * @param paramcontainer The VisParamContainer that needs to be destroyed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_container_destroy (VisParamContainer *paramcontainer)
{
	visual_log_return_val_if_fail (paramcontainer != NULL, -1);

	visual_list_destroy_elements (&paramcontainer->entries, param_list_destroy);

	visual_mem_free (paramcontainer);

	return 0;
}

/**
 * Sets the eventqueue in the VisParamContainer, so events can be emitted on param changes.
 *
 * @param paramcontainer A pointer to the VisParamContainer to which the VisEventQueue needs to be set.
 * @param eventqueue A Pointer to the VisEventQueue that is used for the events the VisParamContainer can emit.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue)
{
	visual_log_return_val_if_fail (paramcontainer != NULL, -1);

	paramcontainer->eventqueue = eventqueue;

	return 0;
}

/**
 * Get the pointer to the VisEventQueue the VisParamContainer is emitting events to.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which the VisEventQueue is requested.
 * 
 * @return Pointer to the VisEventQueue possibly NULL, NULL on error.
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
 * @return 0 on succes -1 on error.
 */
int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param)
{
	visual_log_return_val_if_fail (paramcontainer != NULL && param != NULL, -1);

	param->parent = paramcontainer;
	
	visual_list_add (&paramcontainer->entries, param);

	return 0;
}

/**
 * Removes a VisParamEntry from the VisParamContainer by giving the name of the VisParamEntry that needs
 * to be removed.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry needs to be removed.
 * @param name The name of the VisParamEntry that needs to be removed from the VisParamContainer.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_container_remove (VisParamContainer *paramcontainer, const char *name)
{
	VisListEntry *le = NULL;
	VisParamEntry *param;

	visual_log_return_val_if_fail (paramcontainer != NULL && name != NULL, -1);
	
	while ((param = visual_list_next (&paramcontainer->entries, &le)) != NULL) {

		if (strcmp (param->name, name) == 0) {
			visual_list_delete (&paramcontainer->entries, &le);

			return 0;
		}
	}

	return -1;
}

/**
 * Retrieve a VisParamEntry from a VisParamContainer by giving the name of the VisParamEntry that is requested.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry is requested.
 * @param name The name of the VisParamEntry that is requested from the VisParamContainer.
 *
 * @return Pointer to the VisParamEntry, or NULL.
 */
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, const char *name)
{
	VisListEntry *le = NULL;
	VisParamEntry *param;

	visual_log_return_val_if_fail (paramcontainer != NULL && name != NULL, NULL);

	while ((param = visual_list_next (&paramcontainer->entries, &le)) != NULL) {
		param = le->data;
		
		if (strcmp (param->name, name) == 0)
			return param;
	}
	
	return NULL;
}

/**
 * Creates a new VisParamEntry structure.
 *
 * @param name The name that is assigned to the VisParamEntry.
 *
 * @return A newly allocated VisParamEntry structure.
 */
VisParamEntry *visual_param_entry_new (char *name)
{
	VisParamEntry *param;

	param = visual_mem_new0 (VisParamEntry, 1);

	param->name = name;

	return param;
}

/**
 * Frees the VisParamEntry. This frees the VisParamEntry data structure.
 *
 * @param param Pointer to the VisParamEntry that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_free (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	if (param->string != NULL)
		visual_mem_free (param->string);

	visual_list_destroy_elements (&param->callbacks, free);

	visual_mem_free (param);

	return 0;
}

/**
 * Adds a change notification callback, this shouldn't be used to get notificated within a plugin, but is for
 * things like VisUI.
 *
 * @param param Pointer to the VisParamEntry to which a change notification callback is added.
 * @param callback The notification callback, which is called on changes in the VisParamEntry.
 * @param priv A private that can be used in the callback function.
 *
 * return 0 on succes -1 on error.
 */
int visual_param_entry_add_callback (VisParamEntry *param, param_changed_callback_func_t callback, void *priv)
{
	VisParamEntryCallback *pcall;

	visual_log_return_val_if_fail (param != NULL, -1);
	visual_log_return_val_if_fail (callback != NULL, -1);

	pcall = visual_mem_new0 (VisParamEntryCallback, 1);

	pcall->callback = callback;
	pcall->priv = priv;

	visual_list_add (&param->callbacks, pcall);

	return 0;
}

/**
 * Removes a change notification callback from the list of callbacks.
 *
 * @param param Pointer to the VisParamEntry from which a change notification callback is removed.
 * @param callback The callback that needs to be removed from the list of callbacks that are called
 * 	on param change.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_remove_callback (VisParamEntry *param, param_changed_callback_func_t callback)
{
	VisListEntry *le = NULL;
	VisParamEntryCallback *pcall;

	visual_log_return_val_if_fail (param != NULL, -1);
	visual_log_return_val_if_fail (callback != NULL, -1);

	while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL) {
		
		if (callback == pcall->callback) {
			visual_list_delete (&param->callbacks, &le);

			visual_mem_free (pcall);

			return 0;
		}
	}

	return 0;
}

/**
 * Notifies all the callbacks for the given VisParamEntry parameter.
 *
 * @param param Pointer to the VisParamEntry of which all the change notification
 * 	callbacks need to be called.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_notify_callbacks (VisParamEntry *param)
{
	VisListEntry *le = NULL;
	VisParamEntryCallback *pcall;

	visual_log_return_val_if_fail (param != NULL, -1);

	while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL)
		pcall->callback (param, pcall->priv);

	return 0;
}

/**
 * Checks if the VisParamEntry it's name is the given name.
 *
 * @param param Pointer to the VisParamEntry of which we want to check the name.
 * @param name The name we want to check against.
 *
 * @return TRUE if the VisParamEntry is the one we requested, or FALSE if not.
 */
int visual_param_entry_is (VisParamEntry *param, const char *name)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	if (strcmp (param->name, name) == 0)
		return TRUE;

	return FALSE;
}

/**
 * When called, emits an event in the VisParamContainer it's VisEventQueue when the VisEventQueue
 * is set.
 * 
 * @param param Pointer to the VisParamEntry that is changed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_changed (VisParamEntry *param)
{
	VisEventQueue *eventqueue;

	visual_log_return_val_if_fail (param != NULL, -1);

	if (param->parent == NULL)
		return 0;

	eventqueue = param->parent->eventqueue;

	if (eventqueue != NULL)
		visual_event_queue_add_param (eventqueue, param);

	visual_param_entry_notify_callbacks (param);

	return 0;
}

/**
 * Copies the value of the src param into the param. Also sets the param to the type of which the
 * source param is.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param src Pointer to the VisParamEntry from which the value is retrieved.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src)
{
	visual_log_return_val_if_fail (param != NULL, -1);
	visual_log_return_val_if_fail (src != NULL, -1);

	switch (src->type) {
		case VISUAL_PARAM_TYPE_NULL:

			break;

		case VISUAL_PARAM_TYPE_STRING:
			visual_param_entry_set_string (param, visual_param_entry_get_string (src));
			break;

		case VISUAL_PARAM_TYPE_INTEGER:
			visual_param_entry_set_integer (param, visual_param_entry_get_integer (src));
			
			break;

		case VISUAL_PARAM_TYPE_FLOAT:
			visual_param_entry_set_float (param, visual_param_entry_get_float (src));

			break;

		case VISUAL_PARAM_TYPE_DOUBLE:
			visual_param_entry_set_double (param, visual_param_entry_get_double (src));

			break;

		case VISUAL_PARAM_TYPE_COLOR:
			visual_param_entry_set_color_by_color (param, visual_param_entry_get_color (src));
			break;


		default:
			visual_log (VISUAL_LOG_CRITICAL, "param type is not valid");

			break;
	}
	
	return 0;
}

/**
 * Set the name for a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry to which the name is set.
 * @param name The name that is set to the VisParamEntry.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_name (VisParamEntry *param, char *name)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->name = name;

	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_STRING and assigns the string given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param string The string for this parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_string (VisParamEntry *param, char *string)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_STRING;

	if (string == NULL && param->string == NULL)
		return 0;

	if (string == NULL && param->string != NULL) {
		param->string = string;

		visual_param_entry_changed (param);

	} else if (param->string == NULL && string != NULL) {
		param->string = string;

		visual_param_entry_changed (param);

	} else if (strcmp (string, param->string) != 0) {
		param->string = strdup (string);

		visual_param_entry_changed (param);
	}

	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_INTEGER and assigns the integer given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param integer The integer value for this parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_integer (VisParamEntry *param, int integer)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_INTEGER;

	if (param->numeric.integer != integer) {
		param->numeric.integer = integer;

		visual_param_entry_changed (param);
	}

	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_FLOAT and assigns the float given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The float value for this parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_float (VisParamEntry *param, float floating)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_FLOAT;

	if (param->numeric.floating != floating) {
		param->numeric.floating = floating;

		visual_param_entry_changed (param);
	}
	
	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_DOUBLE and assigns the double given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param doubleflt The double value for this parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_DOUBLE;

	if (param->numeric.doubleflt != doubleflt) {
		param->numeric.doubleflt = doubleflt;

		visual_param_entry_changed (param);
	}

	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_COLOR and assigns the rgb values given as arguments to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param r The red value for this color parameter.
 * @param g The green value for this color parameter.
 * @param b The blue value for this color parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_COLOR;

	if (param->color.r != r || param->color.g != g || param->color.b != b) {
		param->color.r = r;
		param->color.g = g;
		param->color.b = b;

		visual_param_entry_changed (param);
	}

	return 0;
}

/**
 * Sets the VisParamEntry to VISUAL_PARAM_TYPE_COLOR and assigns the rgb values from the given VisColor as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param color Pointer to the VisColor from which the rgb values are copied into the parameter.
 *
 * @return 0 on succes -1 on error.
 */
int visual_param_entry_set_color_by_color (VisParamEntry *param, const VisColor *color)
{
	visual_log_return_val_if_fail (param != NULL, -1);

	param->type = VISUAL_PARAM_TYPE_COLOR;

	if (visual_color_compare (&param->color, color) == 1) {
		visual_color_copy (&param->color, color);

		visual_param_entry_changed (param);
	}

	return 0;
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

	if (param->type != VISUAL_PARAM_TYPE_STRING) {
		visual_log (VISUAL_LOG_WARNING, "Requesting string from a non string param\n");

		return NULL;
	}

	return param->string;
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

	if (param->type != VISUAL_PARAM_TYPE_INTEGER)
		visual_log (VISUAL_LOG_WARNING, "Requesting integer from a non integer param\n");

	return param->numeric.integer;
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

	if (param->type != VISUAL_PARAM_TYPE_FLOAT)
		visual_log (VISUAL_LOG_WARNING, "Requesting float from a non float param\n");

	return param->numeric.floating;
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

	if (param->type != VISUAL_PARAM_TYPE_DOUBLE)
		visual_log (VISUAL_LOG_WARNING, "Requesting double from a non double param\n");

	return param->numeric.doubleflt;
}

/**
 * Get the color parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the color parameter is requested.
 *
 * @return Pointer to the VisColor parameter from the VisParamEntry. It's adviced to
 * use the VisColor that is returned as read only seen changing it directly won't emit events and
 * can cause synchronous problems between the plugin and the parameter system. Instead use the
 * visual_param_entry_set_color* methods to change the parameter value.
 */
VisColor *visual_param_entry_get_color (VisParamEntry *param)
{
	visual_log_return_val_if_fail (param != NULL, 0);

	if (param->type != VISUAL_PARAM_TYPE_COLOR)
		visual_log (VISUAL_LOG_WARNING, "Requesting color from a non color param\n");

	return &param->color;
}

/**
 * @}
 */

