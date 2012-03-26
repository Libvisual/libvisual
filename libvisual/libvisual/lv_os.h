/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_os.h,v 1.3 2006/01/26 15:13:37 synap Exp $
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

#ifndef _LV_OS_H
#define _LV_OS_H

#include <libvisual/lv_defines.h>

/**
 * @defgroup VisOS VisOS
 * @{
 */

VISUAL_BEGIN_DECLS

/**
 * Puts the process in soft realtime mode. Be very careful with using this, it's very much possible to lock your
 * system up. Only works as super user.
 */
int visual_os_scheduler_realtime_start (void);

/**
 * Returns to normal execution mode. Only works as super user.
 */
int visual_os_scheduler_realtime_stop (void);

/**
 * Yield the process. Don't rely on this.
 */
int visual_os_scheduler_yield (void);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_OS_H */
