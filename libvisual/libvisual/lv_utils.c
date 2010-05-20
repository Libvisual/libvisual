/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_utils.c,v 1.3.2.1 2006/03/04 12:32:47 descender Exp $
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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "lv_utils.h"

/**
 * @defgroup VisUtils VisUtils
 * @{
 */

/**
 * Checks if the given value is a power of 2.
 *
 * @param n Value to be checked if it's being a power of 2.
 *
 * @return TRUE if power of 2, FALSE if not.
 */
int visual_utils_is_power_of_2 (int n)
{
	int bits_found = FALSE;

	if (n < 1)
		return FALSE;

	do {
		if (n & 1) {
			if (bits_found)
				return FALSE;

			bits_found = TRUE;
		}

		n >>= 1;

	} while (n > 0);

	/* Couldn't we use this: */
	/* FIXME: (n & (n -1)) == 0) */

	return TRUE;
}

/**
 * @}
 */
