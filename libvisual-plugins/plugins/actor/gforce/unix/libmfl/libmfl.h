/* libmfl.h - This is the header file for the external interface of LibMFL
 *            the minimal font library.  Programs using the
 *            library should #include this file.
 *
 * Copyright (C) 2000 Boris Gjenero
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License, version 2.1, as published by the Free Software Foundation
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * You may contact Boris Gjenero at bgjenero@sympatico.ca
 */

#ifndef _LIBMFL_USER_H_
#define _LIBMFL_USER_H_

#if defined(__cplusplus) || defined(c_plusplus)
// This is a C library, and this allows it to be used in C++ programs
extern "C" {
#endif

#define MFL_NORMAL 0
#define MFL_XOR 1
#define MFL_OR 2
#define MFL_SETALL 3

struct mfl_font_s;
typedef struct mfl_font_s *mfl_font;

struct mfl_context_s;
typedef struct mfl_context_s *mfl_context;

/* Font management */
mfl_font mfl_LoadRawFont(const char *fname);
void mfl_DestroyFont(mfl_font f);

/* Context management */
mfl_context mfl_CreateContext(void *buf, unsigned int bpp,
			      unsigned int bpl, unsigned int width,
			      unsigned int height);
void mfl_SetTextColor(mfl_context cx, unsigned long c);
void mfl_SetFont(mfl_context cx, mfl_font f);
void mfl_DestroyContext(mfl_context cx);
void mfl_SetDrawMode(mfl_context cx, int mode);

/* Info */
unsigned int mfl_GetTextWidth(const mfl_context cx,
				     const char *s);
unsigned int mfl_GetTextWidthL(const mfl_context cx,
				      const char *s, int l);

/* Character drawing */
void mfl_OutChar8(const mfl_context cx, int x, int y, char c);

/* String drawing */
void mfl_OutText8L(const mfl_context cx, int x, int y,
		   const char *s, int l);
void mfl_OutText8(const mfl_context cx, int x, int y,
		  const char *s);

#if defined(__cplusplus) || defined(c_plusplus)
// This is a C library, and this allows it to be used in C++ programs
}
#endif

#endif
