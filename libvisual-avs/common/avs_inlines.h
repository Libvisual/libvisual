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
#ifndef _INLINES_H_
#define _INLINES_H_

typedef struct {
    unsigned char   blendtable[256][256];
    int reset_vars_on_recompile;
    int line_blend_mode;
    char path[];
} Generic;


static int __inline max(x, y)
{
    return x > y ? x : y;
}

static int __inline min(x, y)
{
    return x < y ? x : y;
}

// inlines
static unsigned int __inline BLEND(unsigned int a, unsigned int b)
{
	register unsigned int r,t;
	r=(a&0xff)+(b&0xff);
	t=min(r,0xff);
	r=(a&0xff00)+(b&0xff00);
	t|=min(r,0xff00);
	r=(a&0xff0000)+(b&0xff0000);
	t|=min(r,0xff0000);
	r=(a&0xff000000)+(b&0xff000000);
	return t|min(r,0xff000000);
}

#define FASTMAX(x,y) max(x,y)
#define FASTMIN(x,y) min(x,y)

static unsigned int __inline BLEND_MAX(unsigned int a, unsigned int b)
{
	register unsigned int t;
  int _a=a&0xff;
  int _b=b&0xff;
	t=FASTMAX(_a,_b);
  _a=a&0xff00; _b=b&0xff00;
  t|=FASTMAX(_a,_b);
  _a=a&0xff0000; _b=b&0xff0000;
  t|=FASTMAX(_a,_b);
	return t;
}

static unsigned int __inline BLEND_MIN(unsigned int a, unsigned int b)
{
	register unsigned int t;
  int _a=a&0xff;
  int _b=b&0xff;
	t=FASTMIN(_a,_b);
  _a=a&0xff00; _b=b&0xff00;
  t|=FASTMIN(_a,_b);
  _a=a&0xff0000; _b=b&0xff0000;
  t|=FASTMIN(_a,_b);
	return t;
}


static unsigned int __inline BLEND_AVG(unsigned int a, unsigned int b)
{
	return ((a>>1)&~((1<<7)|(1<<15)|(1<<23)))+((b>>1)&~((1<<7)|(1<<15)|(1<<23)));
}


static unsigned int __inline BLEND_SUB(unsigned int a, unsigned int b)
{
	register int r,t;
	r=(a&0xff)-(b&0xff);
	t=max(r,0);
	r=(a&0xff00)-(b&0xff00);
	t|=max(r,0);
	r=(a&0xff0000)-(b&0xff0000);
	t|=max(r,0);
	r=(a&0xff000000)-(b&0xff000000);
	return t|max(r,0);
}

static unsigned int __inline BLEND_ADJ(Generic *obj, unsigned int a, unsigned int b, int v)
{
	register int t;
	t=obj->blendtable[a&0xFF][v]+obj->blendtable[b&0xFF][0xFF-v];
	t|=(obj->blendtable[(a&0xFF00)>>8][v]+obj->blendtable[(b&0xFF00)>>8][0xFF-v])<<8;
	t|=(obj->blendtable[(a&0xFF0000)>>16][v]+obj->blendtable[(b&0xFF0000)>>16][0xFF-v])<<16;
	return t;
}

static unsigned int __inline BLEND_MUL(Generic *obj, unsigned int a, unsigned int b)
{
	register int t;
	t=obj->blendtable[a&0xFF][b&0xFF];
	t|=obj->blendtable[(a&0xFF00)>>8][(b&0xFF00)>>8]<<8;
  t|=obj->blendtable[(a&0xFF0000)>>16][(b&0xFF0000)>>16]<<16;
	return t;
}

static __inline void BLEND_LINE(Generic *obj, int *fb, int color)
{
  register int bm=obj->line_blend_mode&0xff;
  switch (obj->line_blend_mode&0xff)
  {
    case 1: *fb=BLEND(*fb,color); break;
    case 2: *fb=BLEND_MAX(*fb,color); break;
    case 3: *fb=BLEND_AVG(*fb,color); break;
    case 4: *fb=BLEND_SUB(*fb,color); break;
    case 5: *fb=BLEND_SUB(color,*fb); break;
    case 6: *fb=BLEND_MUL(obj, *fb,color); break;
    case 7: *fb=BLEND_ADJ(obj, *fb,color,(obj->line_blend_mode>>8)&0xff); break;
    case 8: *fb=*fb^color; break;
    case 9: *fb=BLEND_MIN(*fb,color); break;
    default: *fb=color; break;
  }
}

static __inline unsigned int BLEND4(Generic *obj, unsigned int *p1, unsigned int w, int xp, int yp)
{
  register int t;
  unsigned char a1,a2,a3,a4;
  a1=obj->blendtable[255-xp][255-yp];
  a2=obj->blendtable[xp][255-yp];
  a3=obj->blendtable[255-xp][yp];
  a4=obj->blendtable[xp][yp];
  t=obj->blendtable[p1[0]&0xff][a1]+obj->blendtable[p1[1]&0xff][a2]+obj->blendtable[p1[w]&0xff][a3]+obj->blendtable[p1[w+1]&0xff][a4];
  t|=(obj->blendtable[(p1[0]>>8)&0xff][a1]+obj->blendtable[(p1[1]>>8)&0xff][a2]+obj->blendtable[(p1[w]>>8)&0xff][a3]+obj->blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(obj->blendtable[(p1[0]>>16)&0xff][a1]+obj->blendtable[(p1[1]>>16)&0xff][a2]+obj->blendtable[(p1[w]>>16)&0xff][a3]+obj->blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;      
}


static __inline unsigned int BLEND4_16(Generic *obj, unsigned int *p1, unsigned int w, int xp, int yp)
{
  register int t;
  unsigned char a1,a2,a3,a4;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=obj->blendtable[255-xp][255-yp];
  a2=obj->blendtable[xp][255-yp];
  a3=obj->blendtable[255-xp][yp];
  a4=obj->blendtable[xp][yp];
  t=obj->blendtable[p1[0]&0xff][a1]+obj->blendtable[p1[1]&0xff][a2]+obj->blendtable[p1[w]&0xff][a3]+obj->blendtable[p1[w+1]&0xff][a4];
  t|=(obj->blendtable[(p1[0]>>8)&0xff][a1]+obj->blendtable[(p1[1]>>8)&0xff][a2]+obj->blendtable[(p1[w]>>8)&0xff][a3]+obj->blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(obj->blendtable[(p1[0]>>16)&0xff][a1]+obj->blendtable[(p1[1]>>16)&0xff][a2]+obj->blendtable[(p1[w]>>16)&0xff][a3]+obj->blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;      
}

/*
static __inline void mmx_avgblend_block(int *output, int *input, int l)
{
  while (l--)
  {
    *output=BLEND_AVG(*input++,*output);
    output++;
  }
}


static __inline void mmx_addblend_block(int *output, int *input, int l)
{
  while (l--)
  {
    *output=BLEND(*input++,*output);
    output++;
  }
}

static __inline void mmx_mulblend_block(Generic *obj, int *output, int *input, int l)
{
  while (l--)
  {
    *output=BLEND_MUL(obj, *input++,*output);
    output++;
  }
}

static void __inline mmx_adjblend_block(int *o, int *in1, int *in2, int len, int v)
{
  while (len--)
  {
    *o++=BLEND_ADJ(*in1++,*in2++,inblendval);
  }
}
*/

#endif
