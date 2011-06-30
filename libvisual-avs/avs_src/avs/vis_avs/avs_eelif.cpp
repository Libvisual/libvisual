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
#include <windows.h>
#include "avs_eelif.h"
#include "../ns-eel/ns-eel-addfuncs.h"

#ifdef AVS_MEGABUF_SUPPORT
#include "../ns-eel/megabuf.h"
#endif

static void gmegabuf_cleanup();
void _asm_gmegabuf(void);
void _asm_gmegabuf_end(void);



char last_error_string[1024];
int g_log_errors;
CRITICAL_SECTION g_eval_cs;
static char *g_evallib_visdata;



/////////////////////// begin AVS specific script functions



static double NSEEL_CGEN_CALL getvis(unsigned char *visdata, int bc, int bw, int ch, int xorv)
{
  int x;
  int accum=0;
  if (ch && ch != 1 && ch != 2) return 0.0;

  if (bw < 1) bw=1;
  bc-=bw/2;
  if (bc < 0) 
  {
    bw+=bc;
    bc=0;
  }
  if (bc > 575) bc=575;
  if (bc+bw > 576) bw=576-bc;


  if (!ch)
  {
    for (x = 0; x < bw; x ++) 
    {
      accum+=(visdata[bc]^xorv)-xorv;
      accum+=(visdata[bc+576]^xorv)-xorv;
      bc++;
    }
    return (double)accum / ((double)bw*255.0);
  }
  else 
  {
    if (ch == 2) visdata+=576;
    for (x = 0; x < bw; x ++) accum+=(visdata[bc++]^xorv)-xorv;
    return (double)accum / ((double)bw*127.5);
  }
}

static double NSEEL_CGEN_CALL  getspec_(double *band, double *bandw, double *chan)
{
  if (!g_evallib_visdata) return 0.0;
  return getvis((unsigned char *)g_evallib_visdata,(int)(*band*576.0),(int)(*bandw*576.0),(int)(*chan+0.5),0)*0.5;
}

static double NSEEL_CGEN_CALL getosc_(double *band, double *bandw, double *chan)
{
  if (!g_evallib_visdata) return 0.0;
  return getvis((unsigned char *)g_evallib_visdata+576*2,(int)(*band*576.0),(int)(*bandw*576.0),(int)(*chan+0.5),128);
}

static double NSEEL_CGEN_CALL gettime_(double *sc)
{
  int ispos;
  if ((ispos=(*sc > -1.001 && *sc < -0.999)) || (*sc > -2.001 && *sc < -1.999))
  {
    int pos=0;

    extern HWND hwnd_WinampParent;
	  if (IsWindow(hwnd_WinampParent)) 
    {
      if (!SendMessageTimeout( hwnd_WinampParent, WM_USER,(WPARAM)!ispos,(LPARAM)105,SMTO_BLOCK,50,(LPDWORD)&pos)) pos=0;
    }
    if (!ispos) return (double)pos;
    return pos / 1000.0;
  }

  return GetTickCount()/1000.0 - *sc;
}

static double NSEEL_CGEN_CALL setmousepos_(double *x, double *y)
{
  //fucko: implement me
  return 0.0;
}


extern double DDraw_translatePoint(POINT p, int isY);

static double NSEEL_CGEN_CALL getmouse_(double *which)
{
  int w=(int)(*which+0.5);

  if (w > 5)
    return (GetAsyncKeyState(w)&0x8000)?1.0:0.0;

  if (w == 1 || w == 2)
  {
    POINT p;
    GetCursorPos(&p);
    return DDraw_translatePoint(p,w==2);
  }
  if (w == 3) return (GetAsyncKeyState(MK_LBUTTON)&0x8000)?1.0:0.0;
  if (w == 4) return (GetAsyncKeyState(MK_RBUTTON)&0x8000)?1.0:0.0;
  if (w == 5) return (GetAsyncKeyState(MK_MBUTTON)&0x8000)?1.0:0.0;
  return 0.0;
}

static double (NSEEL_CGEN_CALL *__getosc)(double *,double *,double *) = &getosc_;
__declspec ( naked ) void _asm_getosc(void)
{
  FUNC3_ENTER

  *__nextBlock = __getosc(parm_c,parm_b,parm_a);

  FUNC_LEAVE
}
__declspec ( naked ) void _asm_getosc_end(void) {}


static double (NSEEL_CGEN_CALL *__getspec)(double *,double *,double *) = &getspec_;
__declspec ( naked ) void _asm_getspec(void)
{
  FUNC3_ENTER

  *__nextBlock = __getspec(parm_c,parm_b,parm_a);

  FUNC_LEAVE
}
__declspec ( naked ) void _asm_getspec_end(void) {}

static double (NSEEL_CGEN_CALL *__gettime)(double *) = &gettime_;
__declspec ( naked ) void _asm_gettime(void)
{
  FUNC1_ENTER
  
  *__nextBlock = __gettime(parm_a);

  FUNC_LEAVE
}
__declspec ( naked ) void _asm_gettime_end(void) {}


static double (NSEEL_CGEN_CALL *__getmouse)(double *) = &getmouse_;
__declspec ( naked ) void _asm_getmouse(void)
{
  FUNC1_ENTER
  
  *__nextBlock = __getmouse(parm_a);

  FUNC_LEAVE
}
__declspec ( naked ) void _asm_getmouse_end(void) {}


static double (NSEEL_CGEN_CALL *__setmousepos)(double *,double *) = &setmousepos_;
__declspec ( naked ) void _asm_setmousepos(void)
{
  FUNC2_ENTER
  
  *__nextBlock = __setmousepos(parm_a,parm_b);

  FUNC_LEAVE
}
__declspec ( naked ) void _asm_setmousepos_end(void) {}



/////////////////////// end AVS specific script functions

void AVS_EEL_IF_init()
{
  InitializeCriticalSection(&g_eval_cs);
  NSEEL_init();
  NSEEL_addfunction("getosc",3,(int)_asm_getosc,(int)_asm_getosc_end-(int)_asm_getosc);
  NSEEL_addfunction("getspec",3,(int)_asm_getspec,(int)_asm_getspec_end-(int)_asm_getspec);
  NSEEL_addfunction("gettime",1,(int)_asm_gettime,(int)_asm_gettime_end-(int)_asm_gettime);
  NSEEL_addfunction("getkbmouse",1,(int)_asm_getmouse,(int)_asm_getmouse_end-(int)_asm_getmouse);
  NSEEL_addfunction("setmousepos",2,(int)_asm_setmousepos,(int)_asm_setmousepos_end-(int)_asm_setmousepos);
#ifdef AVS_MEGABUF_SUPPORT
    NSEEL_addfunctionex("megabuf",1,(int)_asm_megabuf,(int)_asm_megabuf_end-(int)_asm_megabuf,megabuf_ppproc);
    NSEEL_addfunction("gmegabuf",1,(int)_asm_gmegabuf,(int)_asm_gmegabuf_end-(int)_asm_gmegabuf);
#endif
}
void AVS_EEL_IF_quit()
{
  gmegabuf_cleanup();
  DeleteCriticalSection(&g_eval_cs);
  NSEEL_quit();
}

static void movestringover(char *str, int amount)
{
  char tmp[1024+8];

  int l=(int)strlen(str);
  l=min(1024-amount-1,l);

  memcpy(tmp,str,l+1);

  while (l >= 0 && tmp[l]!='\n') l--;
  l++;

  tmp[l]=0;//ensure we null terminate

  memcpy(str+amount,tmp,l+1);
}

int AVS_EEL_IF_Compile(int context, char *code)
{
  NSEEL_CODEHANDLE ret;
  EnterCriticalSection(&g_eval_cs);
  ret=NSEEL_code_compile((NSEEL_VMCTX)context,code);
  if (!ret)
  {
    if (g_log_errors)
    {
      char *expr = NSEEL_code_getcodeerror((NSEEL_VMCTX)context);
      if (expr)
      {
        int l=strlen(expr);
        if (l > 512) l=512;
        movestringover(last_error_string,l+2);
        memcpy(last_error_string,expr,l);
        last_error_string[l]='\r';
        last_error_string[l+1]='\n';
      }
    }
  }
  LeaveCriticalSection(&g_eval_cs);
  return (int)ret;
}

void AVS_EEL_IF_Execute(void *handle, char visdata[2][2][576])
{
  if (handle)
  {
    EnterCriticalSection(&g_eval_cs);
    g_evallib_visdata=(char*)visdata;
    NSEEL_code_execute((NSEEL_CODEHANDLE)handle);
    g_evallib_visdata=NULL;
    LeaveCriticalSection(&g_eval_cs);
  }
}

void AVS_EEL_IF_Free(int handle)
{
  NSEEL_code_free((NSEEL_CODEHANDLE)handle);
}


void AVS_EEL_IF_resetvars(NSEEL_VMCTX ctx)
{
#ifdef AVS_MEGABUF_SUPPORT
  megabuf_cleanup(ctx);
#endif
  NSEEL_VM_resetvars(ctx);
}

void AVS_EEL_IF_VM_free(NSEEL_VMCTX ctx)
{
#ifdef AVS_MEGABUF_SUPPORT
  megabuf_cleanup(ctx);
#endif
  NSEEL_VM_free(ctx);
}




//////////////////////////////
static double *gmb_blocks[MEGABUF_BLOCKS];

static void gmegabuf_cleanup()
{
  int x;
  for (x = 0; x < MEGABUF_BLOCKS; x ++)
  {
    if (gmb_blocks[x]) GlobalFree(gmb_blocks[x]);
    gmb_blocks[x]=0;
  }
}

static double * NSEEL_CGEN_CALL gmegabuf_(double *which)
{
  static double error;
  int w=(int)(*which + 0.0001);
  int whichblock = w/MEGABUF_ITEMSPERBLOCK;

  if (w >= 0 && whichblock >= 0 && whichblock < MEGABUF_BLOCKS)
  {
    int whichentry = w%MEGABUF_ITEMSPERBLOCK;
    if (!gmb_blocks[whichblock])
    {
      gmb_blocks[whichblock]=(double *)GlobalAlloc(GPTR,sizeof(double)*MEGABUF_ITEMSPERBLOCK);
    }
    if (gmb_blocks[whichblock])
      return &gmb_blocks[whichblock][whichentry];
  }

  return &error;
}

static double * (NSEEL_CGEN_CALL *__gmegabuf)(double *) = &gmegabuf_;
__declspec ( naked ) void _asm_gmegabuf(void)
{
  double *parm_a, *__nextBlock;
  __asm { mov edx, 0xFFFFFFFF }
  __asm { mov ebp, esp }
  __asm { sub esp, __LOCAL_SIZE }
  __asm { mov dword ptr parm_a, eax }
  
  __nextBlock = __gmegabuf(parm_a);

  __asm { mov eax, __nextBlock } // this is custom, returning pointer
  __asm { mov esp, ebp }
}
__declspec ( naked ) void _asm_gmegabuf_end(void) {}
