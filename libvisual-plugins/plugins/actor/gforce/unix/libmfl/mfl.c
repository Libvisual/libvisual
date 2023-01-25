/* mfl.c - General parts of Minimal Font Library
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libvisual/libvisual.h>
#include "libmfl.h"

#undef MFL_TEST

struct mfl_font_s {
  int height;
  unsigned char *data;
};

struct mfl_context_s {
  void *buf;

  unsigned int bpp;
  unsigned int width;
  unsigned int height;
  unsigned int bpl;

  unsigned long color;
  mfl_font font;
  int opmode;
};

mfl_font mfl_LoadRawFont(const char *fname) {
  mfl_font f = NULL;
  unsigned long int l;
  FILE *ff;

  /* Open font file */
  ff = fopen(fname, "rb");
  if (ff == NULL) {
    visual_log (VISUAL_LOG_WARNING, "Unable to open font file: %s", fname);
    goto lrf_open_fault;
  }

  /* Get length of font file */
  if (fseek(ff, 0, SEEK_END) != 0) goto lrf_fault;
  l = ftell(ff);

  /* Seek to start */
  if (fseek(ff, 0, SEEK_SET) != 0) goto lrf_fault;

  /* Determine font height */
  if (l & 0xff) goto lrf_fault;  /* Unknown length */

  /* Allocate data, fill out structure */
  f = malloc(sizeof(struct mfl_font_s));
  f->height = l >> 8;
  f->data = malloc(l);
 
  /* Read font data */
  if (fread(f->data, 1, l, ff) != l) {
    visual_log (VISUAL_LOG_WARNING, "Unable to fully read font file: %s", fname);
    free(f->data);
    free(f);
    f = NULL;
  }

 lrf_fault:
  /* Close input file */
  fclose(ff);

 lrf_open_fault:
  return f;
}

void mfl_DestroyFont(mfl_font f) {
  if (f == NULL) {
    return;
  }
  free(f->data);
  free(f);
}


mfl_context mfl_CreateContext(void *buf, unsigned int bpp, 
			      unsigned int bpl, unsigned int width,
			      unsigned int height) {
  mfl_context cx = malloc(sizeof(struct mfl_context_s));
  
  cx->buf = buf;
  cx->bpp = bpp;
  cx->bpl = bpl;
  cx->width = width;
  cx->height = height;

  cx->font = NULL;
  cx->opmode = MFL_NORMAL;

  return cx;
}

void mfl_DestroyContext(mfl_context cx) {
  free(cx);
}

void mfl_SetTextColor(mfl_context cx, unsigned long c) {
  cx->color = c;
}

void mfl_SetFont(mfl_context cx, mfl_font f) {
  cx->font = f;
}

void mfl_SetDrawMode(mfl_context cx, int mode) {
  cx->opmode = mode;
}

inline unsigned int mfl_GetTextWidthL(const mfl_context cx, 
				      const char *s, int l) {
  return l * 8;
}

inline unsigned int mfl_GetTextWidth(const mfl_context cx, const char *s) {
  return mfl_GetTextWidthL(cx, s, strlen(s));
}

void mfl_OutChar8(const mfl_context cx, int x, int y, char c) {
  unsigned int i;
  int rows;
  unsigned char smask, j;
  unsigned char *fp;
  unsigned char *dp, *ndp;
  unsigned char *dpe;
  
  if (cx->font == NULL) return;

  /* Setup pointers */
  fp = cx->font->data + (cx->font->height * c);  
  
  if (y < 0) {
    rows = cx->font->height + y;
    fp -= y;
    y = 0;
  } else {
    rows = cx->font->height;
  }
  if (y + rows >= cx->height) {
    rows = cx->height - y;
  }
  if (rows <= 0) return;

  smask = 0x80;
  if (x < 0) {
    smask >>= -x;
    x = 0;
    if (smask == 0) return;
  }

  dp = (unsigned char *)cx->buf + (y * cx->bpl) + x;

  /* Do it */
  for (i = 0; i < rows; i++) {
    dpe = dp + (cx->width - x);
    ndp = dp + cx->bpl;
    for (j = smask; j > 0 && dp < dpe; j >>= 1) {
      if (*fp & j) {
	switch(cx->opmode) {
	case MFL_XOR: *dp ^= cx->color; break;
	case MFL_OR: *dp |= cx->color; break;
	case MFL_SETALL: *dp = 0xff; break;
	  /* If we don't recognize the style, revert to normal */
	default: *dp = cx->color; break;
	}
      }	
      dp++;
    }
    fp++;
    dp = ndp;
  }
}

void mfl_OutText8L(const mfl_context cx, int x, int y, const char *s, int l) {
  const char *esp = s + l;
  
  while (s < esp) {
    mfl_OutChar8(cx, x, y, *s);
    s++;
    x+=8;
  }
}

void mfl_OutText8(const mfl_context cx, int x, int y, const char *s) {
  mfl_OutText8L(cx, x, y, s, strlen(s));
}

#ifdef MFL_TEST
main(int argc, char **argv) {
  #define SCREEN_X 78
  #define SCREEN_Y 22
  #define SCREEN_BPL (SCREEN_X + 1)

  mfl_font f;
  mfl_context c;
  int i;
  int y;

  char screen[SCREEN_BPL * SCREEN_Y + 1];
  
  for (i = 0; i < SCREEN_Y; i++) {
    visual_mem_set(&(screen[i * SCREEN_BPL]), '.', SCREEN_X);
    screen[i * SCREEN_BPL + SCREEN_X] = '\n';
  }
  screen[SCREEN_BPL * SCREEN_Y] = 0;

  f = mfl_LoadRawFont("deffont");
  c = mfl_CreateContext(screen, 8, SCREEN_BPL, SCREEN_X, SCREEN_Y);
  mfl_SetTextColor(c, '*');
  mfl_SetFont(c, f);

  y = 0;
  for (i = 1; i < argc; i++) {
    mfl_OutText8(c, i, y, argv[i]);
    y += 16;
  }

  puts(screen);
}
#endif



