/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_os.c,v 1.2 2006-01-22 13:23:37 synap Exp $
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

#ifdef HAVE_SCHED_H
#include <sched.h>
#elif defined HAVE_SYS_SCHED_H
#include <sys/sched.h>
#endif

#include "lv_os.h"

/**
 * @defgroup VisOS VisOS
 * @{
 */

int visual_os_scheduler_get_priority_max (int policy)
{
	return VISUAL_OK;
}

int visual_os_scheduler_get_priority_min (int policy)
{
	return VISUAL_OK;
}

//int visual_os_scheduler_getparam (struct visual_os_scheduler_param *);
//int visual_os_scheduler_setparam (const struct visual_os_scheduler_param *);

int visual_os_scheduler_get_scheduler ()
{
	return VISUAL_OK;
}

//int visual_os_scheduler_setscheduler (int, const struct visual_os_scheduler_param *);

int visual_os_scheduler_yield ()
{
	return VISUAL_OK;
}


/**
 * @}
 */

