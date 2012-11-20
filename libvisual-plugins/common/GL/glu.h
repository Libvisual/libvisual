/* Taken and adapted from Mesa 8.0.2 */

/*
 * SGI FREE SOFTWARE LICENSE B (Version 2.0, Sept. 18, 2008)
 * Copyright (C) 1991-2000 Silicon Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice including the dates of first publication and
 * either this permission notice or a reference to
 * http://oss.sgi.com/projects/FreeB/
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * SILICON GRAPHICS, INC. BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of Silicon Graphics, Inc.
 * shall not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization from
 * Silicon Graphics, Inc.
 */

#ifndef __glu_h__
#define __glu_h__

#ifdef USE_OPENGL_ES
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef GLAPIENTRY
#if defined(_MSC_VER) || defined(__MINGW32__)
#define GLAPIENTRY __stdcall
#else
#define GLAPIENTRY
#endif
#endif

#ifndef GLAPIENTRYP
#define GLAPIENTRYP GLAPIENTRY *
#endif

#if (defined(_MSC_VER) || defined(__MINGW32__)) && defined(BUILD_GLU32)
# undef GLAPI
# define GLAPI __declspec(dllexport)
#elif (defined(_MSC_VER) || defined(__MINGW32__)) && defined(_DLL)
/* tag specifying we're building for DLL runtime support */
# undef GLAPI
# define GLAPI __declspec(dllimport)
#elif !defined(GLAPI)
/* for use with static link lib build of Win32 edition only */
# define GLAPI extern
#endif /* _STATIC_MESA support */

#ifdef USE_OPENGL_ES
#define _GL_REAL GLfloat
#define _GL_CLAMP_REAL GLclampf
#else
#define _GL_REAL GLdouble
#define _GL_CLAMP_REAL GLclampd
#endif

#ifdef __cplusplus
extern "C" {
#endif
GLAPI void GLAPIENTRY gluLookAt (_GL_REAL eyeX, _GL_REAL eyeY, _GL_REAL eyeZ, _GL_REAL centerX, _GL_REAL centerY, _GL_REAL centerZ, _GL_REAL upX, _GL_REAL upY, _GL_REAL upZ);
GLAPI void GLAPIENTRY gluOrtho2D (_GL_REAL left, _GL_REAL right, _GL_REAL bottom, _GL_REAL top);
GLAPI void GLAPIENTRY gluPerspective (_GL_REAL fovy, _GL_REAL aspect, _GL_REAL zNear, _GL_REAL zFar);
GLAPI void GLAPIENTRY gluPickMatrix (_GL_REAL x, _GL_REAL y, _GL_REAL delX, _GL_REAL delY, GLint *viewport);
GLAPI GLint GLAPIENTRY gluProject (_GL_REAL objX, _GL_REAL objY, _GL_REAL objZ, const _GL_REAL *model, const _GL_REAL *proj, const GLint *view, _GL_REAL* winX, _GL_REAL* winY, _GL_REAL* winZ);
GLAPI GLint GLAPIENTRY gluUnProject (_GL_REAL winX, _GL_REAL winY, _GL_REAL winZ, const _GL_REAL *model, const _GL_REAL *proj, const GLint *view, _GL_REAL* objX, _GL_REAL* objY, _GL_REAL* objZ);
GLAPI GLint GLAPIENTRY gluUnProject4 (_GL_REAL winX, _GL_REAL winY, _GL_REAL winZ, _GL_REAL clipW, const _GL_REAL *model, const _GL_REAL *proj, const GLint *view, _GL_CLAMP_REAL nearVal, _GL_CLAMP_REAL farVal, _GL_REAL* objX, _GL_REAL* objY, _GL_REAL* objZ, _GL_REAL* objW);

#ifdef __cplusplus
}
#endif

#endif /* __glu_h__ */
