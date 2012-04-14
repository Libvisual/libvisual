/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_defines.h,v 1.7 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_DEFINES_H
#define _LV_DEFINES_H

#ifdef __cplusplus
# define LV_C_LINKAGE extern "C"
#else
# define LV_C_LINKAGE
#endif /* __cplusplus */

#ifdef __cplusplus
# define LV_BEGIN_DECLS	LV_C_LINKAGE {
# define LV_END_DECLS	}
#else
# define LV_BEGIN_DECLS
# define LV_END_DECLS
#endif /* __cplusplus */

#ifndef NULL
# ifndef __cplusplus
#   define NULL ((void *) 0)
# else
#   define NULL 0
# endif
#endif /* NULL */

#ifndef FALSE
#define FALSE	(0)
#endif

#ifndef TRUE
#define TRUE	(1)
#endif

/* Compiler specific optimization macros */

#if __GNUC__ >= 3
# define LV_ATTR_MALLOC    __attribute__ ((malloc))
# define LV_ATTR_PACKED    __attribute__ ((packed))
# define LV_LIKELY(x)      __builtin_expect (!!(x), 1)
# define LV_UNLIKELY(x)    __builtin_expect (!!(x), 0)
#else
# define LV_ATTR_MALLOC    /* no malloc */
# define LV_ATTR_PACKED    /* no packed */
# define LV_LIKELY(x)      (x)
# define LV_UNLIKELY(x)    (x)
#endif /* __GNUC__ >= 3 */

/* Compile-time format arguments checking macros */

#if defined __GNUC__
#  define LV_CHECK_PRINTF_FORMAT(a, b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#  define LV_CHECK_PRINTF_FORMAT(a, b) /* no compile-time format string check */
#endif /* __GNUC__ */

/* Symbol visibility macros */

#if defined _WIN32 || defined __CYGWIN__
  #define LV_DLL_IMPORT __declspec(dllimport)
  #define LV_DLL_EXPORT __declspec(dllexport)
  #define LV_DLL_LOCAL
#else
  #ifdef __GNUC__ >= 4
    #define LV_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define LV_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define LV_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define LV_DLL_IMPORT
    #define LV_DLL_EXPORT
    #define LV_DLL_LOCAL
  #endif
#endif

#ifdef libvisual_EXPORTS
  #define LV_API LV_DLL_EXPORT
#else
  #define LV_API LV_DLL_IMPORT
#endif

#define LV_LOCAL LV_DLL_LOCAL

#define LV_PLUGIN_EXPORT LV_DLL_EXPORT

#endif /* _LV_DEFINES_H */
