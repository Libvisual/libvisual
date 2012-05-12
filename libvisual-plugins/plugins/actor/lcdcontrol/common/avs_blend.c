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
#ifndef _R_DEFS_H_
#define _R_DEFS_H_

#include "config.h"
#include "avs_blend.h"

static int max(int x, int y)
{
    return x > y ? x : y;
}

static int  min(int x, int y)
{
    return x < y ? x : y;
}

__inline int  BLEND(int a, int b)
{
	register int r,t;
	r=(a&0xff)+(b&0xff);
	t=min(r,0xff);
	r=(a&0xff00)+(b&0xff00);
	t|=min(r,0xff00);
	r=(a&0xff0000)+(b&0xff0000);
	t|=min(r,0xff0000);
	r=(a&0xff000000)+(b&0xff000000);
	return t|min(r,0xff000000);
}

#if 1
#define FASTMAX(x,y) max(x,y)
// (x-(((x-y)>>(32-1))&(x-y))) // hmm not faster :(
#define FASTMIN(x,y) min(x,y)
//(x+(((y-x)>>(32-1))&(y-x))) 
#else

static int FASTMAX(int x, int y)
{
    int ret;
    asm("mov %0, %%ecx\n\t"
        "mov %1, %%eax\n\t"
        "sub %%eax, %%ecx\n\t"
        "cmc\n\t"
        "and %%edx, %%ecx\n\t"
        "add %%ecx, %%eax" 
        : "=a"(ret) 
        : "m"(x), "m"(y)
        : "memory");
    return ret;
}
static int FASTMIN(int x, int y)
{
    int ret;
    asm("mov %0, %%ecx\n\t"
        "mov %1, %%eax\n\t"
        "sub %%eax, %%ecx\n\t"
        "sbb %%edx, %%edx\n\t"
        "and %%edx, %%ecx\n\t"
        "and %%eax, %%ecx\n\t"
        : "=m"(ret) 
        : "m"(x), "m"(y) 
        : "memory");
    return ret;
}

#endif

__inline int  BLEND_MAX(int a, int b)
{
	register int t;
  int _a=a&0xff;
  int _b=b&0xff;
	t=FASTMAX(_a,_b);
  _a=a&0xff00; _b=b&0xff00;
  t|=FASTMAX(_a,_b);
  _a=a&0xff0000; _b=b&0xff0000;
  t|=FASTMAX(_a,_b);
	return t;
}

__inline int  BLEND_MIN(int a, int b)
{
#if 1
	register int t;
  int _a=a&0xff;
  int _b=b&0xff;
	t=FASTMIN(_a,_b);
  _a=a&0xff00; _b=b&0xff00;
  t|=FASTMIN(_a,_b);
  _a=a&0xff0000; _b=b&0xff0000;
  t|=FASTMIN(_a,_b);
	return t;
#else
  __asm
  {
    mov ecx, [a]
    mov eax, [b]

    and ecx, 0xff
    and eax, 0xff

    mov esi, [a]
    mov ebx, [b]

    sub	ecx, eax
	  sbb	edx, edx

    and esi, 0xff00
    and ebx, 0xff00

	  and	ecx, edx
    sub	esi, ebx

	  sbb	edx, edx
	  add	eax, ecx

	  and	esi, edx
    mov ecx, [a]

	  add	ebx, esi
    and ecx, 0xff0000

    mov esi, [b]
    or eax, ebx

    and esi, 0xff0000

    sub	ecx, esi
	  sbb	edx, edx

	  and	ecx, edx
	  add	esi, ecx
    
    or eax, esi
  }
#endif
}

#ifdef FASTMAX
#undef FASTMAX
#undef FASTMIN
#endif


__inline int  BLEND_AVG(int a, int b)
{
	return ((a>>1)&~((1<<7)|(1<<15)|(1<<23)))+((b>>1)&~((1<<7)|(1<<15)|(1<<23)));
}


__inline int  BLEND_SUB(int a, int b)
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

__inline int  BLEND_ADJ_NOMMX(unsigned char blendtable[256][256], int a, int b, int v)
{
	register int t;
	t=blendtable[a&0xFF][v]+blendtable[b&0xFF][0xFF-v];
	t|=(blendtable[(a&0xFF00)>>8][v]+blendtable[(b&0xFF00)>>8][0xFF-v])<<8;
	t|=(blendtable[(a&0xFF0000)>>16][v]+blendtable[(b&0xFF0000)>>16][0xFF-v])<<16;
	return t;
}

__inline int  BLEND_MUL(unsigned char blendtable[256][256], int a, int b)
{
	register int t;
	t=blendtable[a&0xFF][b&0xFF];
	t|=blendtable[(a&0xFF00)>>8][(b&0xFF00)>>8]<<8;
  t|=blendtable[(a&0xFF0000)>>16][(b&0xFF0000)>>16]<<16;
	return t;
}

__inline void BLEND_LINE(int *fb, int color, unsigned char blendtable[256][256], int mode)
{
  switch (mode)
  {
    case 1: *fb=BLEND(*fb,color); break;
    case 2: *fb=BLEND_MAX(*fb,color); break;
    case 3: *fb=BLEND_AVG(*fb,color); break;
    case 4: *fb=BLEND_SUB(*fb,color); break;
    case 5: *fb=BLEND_SUB(color,*fb); break;
    case 6: *fb=BLEND_MUL(blendtable, *fb,color); break;
    case 7: *fb=BLEND_ADJ_NOMMX(blendtable, *fb,color,(mode>>8)&0xff); break;
    case 8: *fb=*fb^color; break;
    case 9: *fb=BLEND_MIN(*fb,color); break;
    default: *fb=color; break;
  }
}

__inline int BLEND4(unsigned char blendtable[256][256], int *p1, int w, int xp, int yp)
{
#if 1
  register int t;
  unsigned char a1,a2,a3,a4;
  a1=blendtable[255-xp][255-yp];
  a2=blendtable[xp][255-yp];
  a3=blendtable[255-xp][yp];
  a4=blendtable[xp][yp];
  t=blendtable[p1[0]&0xff][a1]+blendtable[p1[1]&0xff][a2]+blendtable[p1[w]&0xff][a3]+blendtable[p1[w+1]&0xff][a4];
  t|=(blendtable[(p1[0]>>8)&0xff][a1]+blendtable[(p1[1]>>8)&0xff][a2]+blendtable[(p1[w]>>8)&0xff][a3]+blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(blendtable[(p1[0]>>16)&0xff][a1]+blendtable[(p1[1]>>16)&0xff][a2]+blendtable[(p1[w]>>16)&0xff][a3]+blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;      
#else
  __asm
  {
    movd mm6, xp
    mov eax, p1

    movd mm7, yp
    mov esi, w

    movq mm4, mmx_blend4_revn
    punpcklwd mm6,mm6

    movq mm5, mmx_blend4_revn
    punpcklwd mm7,mm7

    movd mm0, [eax]
    punpckldq mm6,mm6

    movd mm1, [eax+4]
    punpckldq mm7,mm7

    movd mm2, [eax+esi*4]
    punpcklbw mm0, [mmx_blend4_zero]

    movd mm3, [eax+esi*4+4]
    psubw mm4, mm6

    punpcklbw mm1, [mmx_blend4_zero]
    pmullw mm0, mm4

    punpcklbw mm2, [mmx_blend4_zero]
    pmullw mm1, mm6

    punpcklbw mm3, [mmx_blend4_zero]
    psubw mm5, mm7

    pmullw mm2, mm4
    pmullw mm3, mm6

    paddw mm0, mm1
    // stall (mm0)

    psrlw mm0, 8
    // stall (waiting for mm3/mm2)

    paddw mm2, mm3
    pmullw mm0, mm5

    psrlw mm2, 8
    // stall (mm2)

    pmullw mm2, mm7
    // stall

    // stall (mm2)

    paddw mm0, mm2
    // stall

    psrlw mm0, 8
    // stall

    packuswb mm0, mm0
    // stall

    movd eax, mm0
  }
#endif
}


__inline int BLEND4_16(unsigned char blendtable[256][256], int *p1, int w, int xp, int yp)
{
#if 1
  register int t;
  unsigned char a1,a2,a3,a4;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=blendtable[255-xp][255-yp];
  a2=blendtable[xp][255-yp];
  a3=blendtable[255-xp][yp];
  a4=blendtable[xp][yp];
  t=blendtable[p1[0]&0xff][a1]+blendtable[p1[1]&0xff][a2]+blendtable[p1[w]&0xff][a3]+blendtable[p1[w+1]&0xff][a4];
  t|=(blendtable[(p1[0]>>8)&0xff][a1]+blendtable[(p1[1]>>8)&0xff][a2]+blendtable[(p1[w]>>8)&0xff][a3]+blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(blendtable[(p1[0]>>16)&0xff][a1]+blendtable[(p1[1]>>16)&0xff][a2]+blendtable[(p1[w]>>16)&0xff][a3]+blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;      
#else
  __asm
  {
    movd mm6, xp
    mov eax, p1

    movd mm7, yp
    mov esi, w

    movq mm4, mmx_blend4_revn
    psrlw mm6, 8

    movq mm5, mmx_blend4_revn
    psrlw mm7, 8

    movd mm0, [eax]
    punpcklwd mm6,mm6

    movd mm1, [eax+4]
    punpcklwd mm7,mm7

    movd mm2, [eax+esi*4]
    punpckldq mm6,mm6

    movd mm3, [eax+esi*4+4]
    punpckldq mm7,mm7

    punpcklbw mm0, [mmx_blend4_zero]
    psubw mm4, mm6

    punpcklbw mm1, [mmx_blend4_zero]
    pmullw mm0, mm4

    punpcklbw mm2, [mmx_blend4_zero]
    pmullw mm1, mm6

    punpcklbw mm3, [mmx_blend4_zero]
    psubw mm5, mm7

    pmullw mm2, mm4
    pmullw mm3, mm6

    paddw mm0, mm1
    // stall (mm0)

    psrlw mm0, 8
    // stall (waiting for mm3/mm2)

    paddw mm2, mm3
    pmullw mm0, mm5

    psrlw mm2, 8
    // stall (mm2)

    pmullw mm2, mm7
    // stall

    // stall (mm2)

    paddw mm0, mm2
    // stall

    psrlw mm0, 8
    // stall

    packuswb mm0, mm0
    // stall

    movd eax, mm0
  }
#endif
}

__inline void mmx_avgblend_block(int *output, int *input, int l)
{
#if 1
  while (l--)
  {
    *output=BLEND_AVG(*input++,*output);
    output++;
  }
#else
  static int mask[2]=
  {
    ~((1<<7)|(1<<15)|(1<<23)),
      ~((1<<7)|(1<<15)|(1<<23))
  };
  __asm 
  {
    mov eax, input
    mov edi, output
    mov ecx, l
    shr ecx, 2
    align 16
mmx_avgblend_loop:
    movq mm0, [eax]
    movq mm1, [edi]
    psrlq mm0, 1
    movq mm2, [eax+8]
    psrlq mm1, 1
    movq mm3, [edi+8]
    psrlq mm2, 1
    pand mm0, [mask]
    psrlq mm3, 1
    pand mm1, [mask]
    pand mm2, [mask]
    paddusb mm0, mm1
    pand mm3, [mask]
    add eax, 16
    paddusb mm2, mm3

    movq [edi], mm0
    movq [edi+8], mm2

    add edi, 16

    dec ecx
    jnz mmx_avgblend_loop
    emms
  };
#endif
}


__inline void mmx_addblend_block(int *output, int *input, int l)
{
#if 1
  while (l--)
  {
    *output=BLEND(*input++,*output);
    output++;
  }
#else
  __asm 
  {
    mov eax, input
    mov edi, output
    mov ecx, l
    shr ecx, 2
    align 16
mmx_addblend_loop:
    movq mm0, [eax]
    movq mm1, [edi]
    movq mm2, [eax+8]
    movq mm3, [edi+8]
    paddusb mm0, mm1
    paddusb mm2, mm3
    add eax, 16

    movq [edi], mm0
    movq [edi+8], mm2

    add edi, 16

    dec ecx
    jnz mmx_addblend_loop
    emms
  };
#endif
}

__inline void mmx_mulblend_block(unsigned char blendtable[256][256], int *output, int *input, int l)
{
#if 1
  while (l--)
  {
    *output=BLEND_MUL(blendtable, *input++,*output);
    output++;
  }
#else
  __asm 
  {
    mov eax, input
    mov edi, output
    mov ecx, l
    shr ecx, 1
    align 16
mmx_mulblend_loop:
    movd mm0, [eax]
    movd mm1, [edi]
    movd mm2, [eax+4]
    punpcklbw mm0, [mmx_blend4_zero]
    movd mm3, [edi+4]
    punpcklbw mm1, [mmx_blend4_zero]
    punpcklbw mm2, [mmx_blend4_zero]
    pmullw mm0, mm1
    punpcklbw mm3, [mmx_blend4_zero]
    psrlw mm0, 8
    pmullw mm2, mm3
    packuswb mm0, mm0
    psrlw mm2, 8
    packuswb mm2, mm2
    add eax, 8

    movd [edi], mm0
    movd [edi+4], mm2

    add edi, 8

    dec ecx
    jnz mmx_mulblend_loop
    emms
  };
#endif
}

void __inline mmx_adjblend_block(unsigned char blendtable[256][256], int *o, int *in1, int *in2, int len, int v)
{
#if 1
  while (len--)
  {
    *o++=BLEND_ADJ(blendtable, *in1++,*in2++,v);
  }
#else
  __asm
  {
    movd mm3, [v] // VVVVVVVV
    mov ecx, len

    packuswb mm3, mm3 // 0000HHVV
    mov edx, o

    punpcklwd mm3, mm3 // HHVVHHVV
    mov esi, in1

    movq mm4, [mmx_blend4_revn]
    punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV
    
    pand mm3, [mmx_blendadj_mask]
    mov edi, in2

    shr ecx, 1
    psubw mm4, mm3
    
    align 16
_mmx_adjblend_loop:
    
    movd mm0, [esi]
    
    movd mm1, [edi]
    punpcklbw mm0, [mmx_blend4_zero]
    
    movd mm6, [esi+4]
    punpcklbw mm1, [mmx_blend4_zero]
    
    movd mm7, [edi+4]        
    punpcklbw mm6, [mmx_blend4_zero]

    pmullw mm0, mm3
    punpcklbw mm7, [mmx_blend4_zero]

    pmullw mm1, mm4
    pmullw mm6, mm3

    pmullw mm7, mm4
    paddw mm0, mm1

    paddw mm6, mm7
    add edi, 8

    psrlw mm0, 8
    add esi, 8

    psrlw mm6, 8
    packuswb mm0, mm0
    
    packuswb mm6, mm6
    movd [edx], mm0

    movd [edx+4], mm6
    add edx, 8
    dec ecx
    jnz _mmx_adjblend_loop

    emms
  };
#endif
}
#endif
