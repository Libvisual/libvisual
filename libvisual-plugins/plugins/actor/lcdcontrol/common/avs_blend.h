/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#ifndef _R_BLEND_H_
#define _R_BLEND_H_

#include "config.h"
#include "avs_globals.h"

int  BLEND(int a, int b);
int  BLEND_MAX(int a, int b);
int  BLEND_MIN(int a, int b);
int  BLEND_AVG(int a, int b);
int  BLEND_SUB(int a, int b);
int  BLEND_ADJ_NOMMX(unsigned char blendtable[256][256], int a, int b, int v);
#define BLEND_ADJ BLEND_ADJ_NOMMX
int  BLEND_MUL(unsigned char blendtable[256][256], int a, int b);
void BLEND_LINE(int *fb, int color, unsigned char blendtable[256][256], int mode);
int BLEND4(unsigned char blendtable[256][256], int *p1, int w, int xp, int yp);
int BLEND4_16(unsigned char blendtable[256][256], int *p1, int w, int xp, int yp);
void mmx_mulblend_block(unsigned char blendtable[256][256], int *output, int *input, int l);
void mmx_adjblend_block(unsigned char blendtable[256][256], int *o, int *in1, int *in2, int len, int v);
void mmx_addblend_block(int *output, int *input, int l);
void mmx_avgblend_block(int *output, int *input, int l);

#endif
