#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_common.h"
#include "lv_color.h"

/**
 * @defgroup VisColor VisColor
 * @{
 */

/**
 * Creates a new VisColor structure
 *
 * @return A newly allocated VisColor.
 */
VisColor *visual_color_new ()
{
	VisColor *color;

	color = visual_mem_new0 (VisColor, 1);

	return color;
}

/**
 * Frees the VisColor. This frees the VisColor data structure.
 *
 * @param color Pointer to a VisColor that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_color_free (VisColor *color)
{
	visual_log_return_val_if_fail (color != NULL, -1);

	visual_mem_free (color);

	return 0;
}

/**
 * @}
 */

