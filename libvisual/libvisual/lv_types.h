/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id: lv_types.h,v 1.12 2006-11-26 19:34:44 descender Exp $
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

#ifndef _LV_TYPES_H
#define _LV_TYPES_H

#include <libvisual/lv_defines.h>
#include <libvisual/lvconfig.h>

#include <limits.h>

#if defined(LV_HAVE_STDINT_H)
#  include <stdint.h>
#elif defined(LV_HAVE_INTTYPES_H)
#  include <inttypes.h>
#else
#  if UCHAR_MAX == 0xff
     typedef signed char    int8_t;
     typedef unsigned char  uint8_t;
#  else
#    error Cannot find integral type to use to define int8_t and uint8_t
#  endif
#  if USHRT_MAX == 0xffff
     typedef signed short   int16_t;
     typedef unsigned short uint16_t;
#  else
#    error Cannot find integral type to use to define int16_t and uint16_t
#  endif
#  if ULONG_MAX == 0xffffffff
     typedef signed long    int32_t;
     typedef unsigned long  uint32_t;
#  elif UINT_MAX == 0xffffffff
     typedef signed int     int32_t;
     typedef unsigned int   uint32_t;
#  else
#    error Cannot find integral type to use to define int32_t and uint32_t
#  endif
#endif

#if !defined(UINT64_MAX)
#  if defined(LV_HAVE_LONG_LONG) && LV_LONG_LONG_SIZE == 8
#    if defined(__GNUC__)
       __extension__ typedef long long          int64_t;
       __extension__ typedef unsigned long long uint64_t;
#    else
       typedef long long          int64_t;
       typedef unsigned long long uint64_t;
#    endif
#  elif defined(LV_HAVE_MS_INT64)
     typedef __int64          int64_t;
     typedef unsigned __int64 uint64_t;
#  elif ULONG_MAX != 0xffffffff && ULONG_MAX == 18446744073709551615
     typedef long             int64_t;
     typedef unsigned long    uint64_t;
#  else
#    error Cannot find integral type to use to define int64_t and uint64_t
#  endif
#endif

VISUAL_BEGIN_DECLS

#define VISUAL_CHECK_CAST(uiobj, cast)		((cast*) (uiobj))

#define VISUAL_TABLESIZE(table)			(sizeof (table) / sizeof (table[0]))

VISUAL_END_DECLS

#endif /* _LV_TYPES_H */
