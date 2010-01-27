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
#include "ns-eel-int.h"

#ifdef NSEEL_REENTRANT_EXECUTION
#include <malloc.h>
#endif

static int nseel_evallib_stats[5]; // source bytes, static code bytes, call code bytes, data bytes, segments
int *NSEEL_getstats()
{
  return nseel_evallib_stats;
}
double *NSEEL_getglobalregs()
{
  return nseel_globalregs;
}




#define LLB_DSIZE (65536-64)
typedef struct _llBlock {
	struct _llBlock *next;
  int sizeused;
	char block[LLB_DSIZE];
} llBlock;

typedef struct _startPtr {
  struct _startPtr *next;
  void *startptr;
} startPtr;

typedef struct {
  int workTablePtr_size;

  llBlock *blocks;
  void *code;
  int code_stats[4];
} codeHandleType;

#ifndef NSEEL_MAX_TEMPSPACE_ENTRIES
#define NSEEL_MAX_TEMPSPACE_ENTRIES 2048
#endif

static void *__newBlock(llBlock **start,int size);

#define newTmpBlock(x) __newBlock((llBlock **)&ctx->tmpblocks_head,x)
#define newBlock(x) __newBlock((llBlock **)&ctx->blocks_head,x)

static void freeBlocks(llBlock *start);

#define DECL_ASMFUNC(x)         \
  void nseel_asm_##x##(void);        \
  void nseel_asm_##x##_end(void);    \

  DECL_ASMFUNC(sin)
  DECL_ASMFUNC(cos)
  DECL_ASMFUNC(tan)
  DECL_ASMFUNC(asin)
  DECL_ASMFUNC(acos)
  DECL_ASMFUNC(atan)
  DECL_ASMFUNC(atan2)
  DECL_ASMFUNC(sqr)
  DECL_ASMFUNC(sqrt)
  DECL_ASMFUNC(pow)
  DECL_ASMFUNC(exp)
  DECL_ASMFUNC(log)
  DECL_ASMFUNC(log10)
  DECL_ASMFUNC(abs)
  DECL_ASMFUNC(min)
  DECL_ASMFUNC(min)
  DECL_ASMFUNC(max)
  DECL_ASMFUNC(sig)
  DECL_ASMFUNC(sign)
  DECL_ASMFUNC(rand)
  DECL_ASMFUNC(band)
  DECL_ASMFUNC(bor)
  DECL_ASMFUNC(bnot)
  DECL_ASMFUNC(if)
  DECL_ASMFUNC(repeat)
  DECL_ASMFUNC(equal)
  DECL_ASMFUNC(below)
  DECL_ASMFUNC(above)
  DECL_ASMFUNC(assign)
  DECL_ASMFUNC(add)
  DECL_ASMFUNC(sub)
  DECL_ASMFUNC(mul)
  DECL_ASMFUNC(div)
  DECL_ASMFUNC(mod)
  DECL_ASMFUNC(or)
  DECL_ASMFUNC(and)
  DECL_ASMFUNC(uplus)
  DECL_ASMFUNC(uminus)
  DECL_ASMFUNC(floor)
  DECL_ASMFUNC(ceil)
  DECL_ASMFUNC(invsqrt)
  DECL_ASMFUNC(exec2)


static functionType fnTable1[] = {
													 { "if",     nseel_asm_if,nseel_asm_if_end,    3 },
#ifdef NSEEL_LOOPFUNC_SUPPORT
													 { "loop", nseel_asm_repeat,nseel_asm_repeat_end, 2 },
#endif
                           { "sin",   nseel_asm_sin,nseel_asm_sin_end,   1 },
                           { "cos",    nseel_asm_cos,nseel_asm_cos_end,   1 },
                           { "tan",    nseel_asm_tan,nseel_asm_tan_end,   1 },
                           { "asin",   nseel_asm_asin,nseel_asm_asin_end,  1 },
                           { "acos",   nseel_asm_acos,nseel_asm_acos_end,  1 },
                           { "atan",   nseel_asm_atan,nseel_asm_atan_end,  1 },
                           { "atan2",  nseel_asm_atan2,nseel_asm_atan2_end, 2 },
                           { "sqr",    nseel_asm_sqr,nseel_asm_sqr_end,   1 },
                           { "sqrt",   nseel_asm_sqrt,nseel_asm_sqrt_end,  1 },
                           { "pow",    nseel_asm_pow,nseel_asm_pow_end,   2 },
                           { "exp",    nseel_asm_exp,nseel_asm_exp_end,   1 },
                           { "log",    nseel_asm_log,nseel_asm_log_end,   1 },
                           { "log10",  nseel_asm_log10,nseel_asm_log10_end, 1 },
                           { "abs",    nseel_asm_abs,nseel_asm_abs_end,   1 },
                           { "min",    nseel_asm_min,nseel_asm_min_end,   2 },
                           { "max",    nseel_asm_max,nseel_asm_max_end,   2 },
													 { "sigmoid",nseel_asm_sig,nseel_asm_sig_end,   2 } ,
													 { "sign",   nseel_asm_sign,nseel_asm_sign_end,  1 } ,
													 { "rand",   nseel_asm_rand,nseel_asm_rand_end,  1 } ,
													 { "band",   nseel_asm_band,nseel_asm_band_end,  2 } ,
													 { "bor",    nseel_asm_bor,nseel_asm_bor_end,   2 } ,
													 { "bnot",   nseel_asm_bnot,nseel_asm_bnot_end,  1 } ,
													 { "equal",  nseel_asm_equal,nseel_asm_equal_end, 2 },
													 { "below",  nseel_asm_below,nseel_asm_below_end, 2 },
													 { "above",  nseel_asm_above,nseel_asm_above_end, 2 },
                           { "floor",  nseel_asm_floor,nseel_asm_floor_end, 1 },
                           { "ceil",   nseel_asm_ceil,nseel_asm_ceil_end,  1 },
                           { "invsqrt",   nseel_asm_invsqrt,nseel_asm_invsqrt_end,  1 },
                           { "assign",nseel_asm_assign,nseel_asm_assign_end,2},
                           { "exec2",nseel_asm_exec2,nseel_asm_exec2_end,2},
                           { "exec3",nseel_asm_exec2,nseel_asm_exec2_end,3},
                           };

static functionType *fnTableUser;
static int fnTableUser_size;

functionType *nseel_getFunctionFromTable(int idx)
{
  if (idx<0) return 0;
  if (idx>=sizeof(fnTable1)/sizeof(fnTable1[0]))
  {
    idx -= sizeof(fnTable1)/sizeof(fnTable1[0]);
    if (!fnTableUser || idx >= fnTableUser_size) return 0;
    return fnTableUser+idx;
  }
  return fnTable1+idx;
}

int NSEEL_init() // returns 0 on success
{
  NSEEL_quit();
  return 0;
}

void NSEEL_addfunctionex(char *name, int nparms, int code_startaddr, int code_len, void *pproc)
{
  if (!fnTableUser || !(fnTableUser_size&7))
  {
    fnTableUser=(functionType *)realloc(fnTableUser,(fnTableUser_size+8)*sizeof(functionType));
  }
  if (fnTableUser)
  {
    fnTableUser[fnTableUser_size].nParams = nparms;
    fnTableUser[fnTableUser_size].name = name;
    fnTableUser[fnTableUser_size].afunc = (void *)code_startaddr;
    fnTableUser[fnTableUser_size].func_e = (void *)(code_startaddr + code_len);
    fnTableUser[fnTableUser_size].pProc = (NSEEL_PPPROC) pproc;
    fnTableUser_size++;
  }
}

void NSEEL_quit()
{
  free(fnTableUser);
  fnTableUser_size=0;
  fnTableUser=0;
}

//---------------------------------------------------------------------------------------------------------------
static void *realAddress(void *fn, void *fn_e, int *size)
{
#ifdef DISABLED_DEBUG
  char *ptr = (char *)fn;
  int beg=(*(int *)(ptr+1))+5;

  int extrasize=(int)nseel_asm_exec2_end - (int)nseel_asm_exec2;
  int extrabeg=(*(int *)(((char *)nseel_asm_exec2)+1))+5;

  *size=((int)fn_e - (int)fn) - (extrasize-extrabeg) - beg;
  return ptr + beg;
#else
  // Release Mode
  *size  = (int)fn_e - (int) fn;
	return fn;
#endif
}

//---------------------------------------------------------------------------------------------------------------
static void freeBlocks(llBlock *start)
{
  while (start)
	{
    llBlock *llB = start->next;
    GlobalFree(start);
	  start=llB;
	}
}

//---------------------------------------------------------------------------------------------------------------
static void *__newBlock(llBlock **start, int size)
{
  llBlock *llb;
  int alloc_size;
  if (*start && (LLB_DSIZE - (*start)->sizeused) >= size)
  {
    void *t=(*start)->block+(*start)->sizeused;
    (*start)->sizeused+=size;
    return t;
  }

  alloc_size=sizeof(llBlock);
  if ((int)size > LLB_DSIZE) alloc_size += size - LLB_DSIZE;
  llb = (llBlock *)GlobalAlloc(GMEM_FIXED,alloc_size); // grab bigger block if absolutely necessary (heh)
  llb->sizeused=size;
  llb->next = *start;  
  *start = llb;
  return llb->block;
}


#define X86_MOV_EAX_DIRECTVALUE 0xB8
#define X86_MOV_ESI_DIRECTVALUE 0xBE
#define X86_MOV_ESI_DIRECTMEMVALUE 0x358B
#define X86_PUSH_EAX 0x50
#define X86_POP_EBX 0x5B
#define X86_POP_ECX 0x59
#define X86_MOV_ESI_EDI 0xF78B

#define X86_PUSH_ESI 0x56
#define X86_POP_ESI  0x5E

#define X86_RET 0xC3


//---------------------------------------------------------------------------------------------------------------
static int *findFBlock(char *p)
{
  while (*(int *)p != 0xFFFFFFFF) p++;
  return (int*)p;
}


//---------------------------------------------------------------------------------------------------------------
int nseel_createCompiledValue(compileContext *ctx, double value, double *addrValue)
{
  unsigned char *block;
  double *dupValue;

  block=(unsigned char *)newTmpBlock(4+5);

  if (addrValue == NULL)
  {
    ctx->l_stats[3]+=sizeof(double);
	  *(dupValue = (double *)newBlock(sizeof(double))) = value;
  }
  else
	  dupValue = addrValue;

  ((int*)block)[0]=5;
  block[4]=X86_MOV_EAX_DIRECTVALUE; // mov eax, <value>
  *(int *)(block+5) = (int)dupValue;

  return ((int)(block));

}

//---------------------------------------------------------------------------------------------------------------
static int nseel_getFunctionAddress(int fntype, int fn, int *size, NSEEL_PPPROC *pProc)
{
  *pProc = NULL;
  switch (fntype)
	{
  	case MATH_SIMPLE:
	  	switch (fn)
			{
			  case FN_ASSIGN:
				  return (int)realAddress(nseel_asm_assign,nseel_asm_assign_end,size);
			  case FN_ADD:
				  return (int)realAddress(nseel_asm_add,nseel_asm_add_end,size);
			  case FN_SUB:
				  return (int)realAddress(nseel_asm_sub,nseel_asm_sub_end,size);
			  case FN_MULTIPLY:
				  return (int)realAddress(nseel_asm_mul,nseel_asm_mul_end,size);
			  case FN_DIVIDE:
				  return (int)realAddress(nseel_asm_div,nseel_asm_div_end,size);
			  case FN_MODULO:
				  return (int)realAddress(nseel_asm_mod,nseel_asm_mod_end,size);
			  case FN_AND:
				  return (int)realAddress(nseel_asm_and,nseel_asm_and_end,size);
			  case FN_OR:
				  return (int)realAddress(nseel_asm_or,nseel_asm_or_end,size);
			  case FN_UPLUS:
				  return (int)realAddress(nseel_asm_uplus,nseel_asm_uplus_end,size);
			  case FN_UMINUS:
				  return (int)realAddress(nseel_asm_uminus,nseel_asm_uminus_end,size);
			}
	  case MATH_FN:
      {
        functionType *p=nseel_getFunctionFromTable(fn);
		    if (!p) 
        {
          if (size) *size=0;
          return 0;
        }
        if (p->pProc) *pProc=p->pProc;
        return (int)realAddress(p->afunc,p->func_e,size);
      }
	}		
  return 0;
}


//---------------------------------------------------------------------------------------------------------------
int nseel_createCompiledFunction3(compileContext *ctx, int fntype, int fn, int code1, int code2, int code3)
{
  int sizes1=((int *)code1)[0];
  int sizes2=((int *)code2)[0];
  int sizes3=((int *)code3)[0];

  if (fntype == MATH_FN && fn == 0) // special case: IF
  {
    void *func3;
    int size;
    int *ptr;
    char *block;

    unsigned char *newblock2,*newblock3;

    newblock2=newBlock(sizes2+1);
    memcpy(newblock2,(char*)code2+4,sizes2);
    newblock2[sizes2]=X86_RET;

    newblock3=newBlock(sizes3+1);
    memcpy(newblock3,(char*)code3+4,sizes3);
    newblock3[sizes3]=X86_RET;

    ctx->l_stats[2]+=sizes2+sizes3+2;
    
    func3 = realAddress(nseel_asm_if,nseel_asm_if_end,&size);

    block=(char *)newTmpBlock(4+sizes1+size);
    ((int*)block)[0]=sizes1+size;
    memcpy(block+4,(char*)code1+4,sizes1);
    ptr=(int *)(block+4+sizes1);
    memcpy(ptr,func3,size);

    ptr=findFBlock((char*)ptr); *ptr++=(int)newblock2;
    ptr=findFBlock((char*)ptr); *ptr=(int)newblock3;
    ctx->computTableTop++;

    return (int)block;

  }
  else
  {
    int size2;
    unsigned char *block;
    unsigned char *outp;

    int myfunc;
    NSEEL_PPPROC preProc;
  
    myfunc = nseel_getFunctionAddress(fntype, fn, &size2, &preProc);

    block=(unsigned char *)newTmpBlock(4+size2+sizes1+sizes2+sizes3+4);

    ((int*)block)[0]=4+size2+sizes1+sizes2+sizes3;
    outp=block+4;
    memcpy(outp,(char*)code1+4,sizes1); 
    outp+=sizes1;
    *outp++ = X86_PUSH_EAX;
    memcpy(outp,(char*)code2+4,sizes2); 
    outp+=sizes2;
    *outp++ = X86_PUSH_EAX;
    memcpy(outp,(char*)code3+4,sizes3); 
    outp+=sizes3;
    *outp++ = X86_POP_EBX;
    *outp++ = X86_POP_ECX;

    memcpy(outp,(void*)myfunc,size2);
    if (preProc) preProc(outp,size2,ctx->userfunc_data);

    ctx->computTableTop++;

    return ((int)(block));  
  }
}

//---------------------------------------------------------------------------------------------------------------
int nseel_createCompiledFunction2(compileContext *ctx, int fntype, int fn, int code1, int code2)
{
  int size2;
  unsigned char *block;
  unsigned char *outp;
  int myfunc;
  int sizes1=((int *)code1)[0];
  int sizes2=((int *)code2)[0];

#ifdef NSEEL_LOOPFUNC_SUPPORT
  if (fntype == MATH_FN && fn == 1) // special case: REPEAT
  {
    void *func3;
    int size;
    int *ptr;
    char *block;

    unsigned char *newblock2;

    newblock2=newBlock(sizes2+1);
    memcpy(newblock2,(char*)code2+4,sizes2);
    newblock2[sizes2]=X86_RET;

    ctx->l_stats[2]+=sizes2+2;
    
    func3 = realAddress(nseel_asm_repeat,nseel_asm_repeat_end,&size);

    block=(char *)newTmpBlock(4+sizes1+size);
    ((int*)block)[0]=sizes1+size;
    memcpy(block+4,(char*)code1+4,sizes1);
    ptr=(int *)(block+4+sizes1);
    memcpy(ptr,func3,size);

    ptr=findFBlock((char*)ptr); *ptr++=(int)newblock2;

    ctx->computTableTop++;
    return (int)block;
  }
  else
#endif
  {
    NSEEL_PPPROC preProc;
    myfunc = nseel_getFunctionAddress(fntype, fn, &size2,&preProc);

    block=(unsigned char *)newTmpBlock(2+size2+sizes1+sizes2+4);

    ((int*)block)[0]=2+size2+sizes1+sizes2;
    outp=block+4;
    memcpy(outp,(char*)code1+4,sizes1); 
    outp+=sizes1;
    *outp++ = X86_PUSH_EAX;
    memcpy(outp,(char*)code2+4,sizes2); 
    outp+=sizes2;
    *outp++ = X86_POP_EBX;

    memcpy(outp,(void*)myfunc,size2);
    if (preProc) preProc(outp,size2,ctx->userfunc_data);

    ctx->computTableTop++;

    return ((int)(block));
  }
}


//---------------------------------------------------------------------------------------------------------------
int nseel_createCompiledFunction1(compileContext *ctx, int fntype, int fn, int code)
{
  NSEEL_PPPROC preProc;
  int size,size2;
  char *block;
  int myfunc;
  void *func1;
  
  size =((int *)code)[0];
  func1 = (void *)(code+4);
  
  myfunc = nseel_getFunctionAddress(fntype, fn, &size2,&preProc);

  block=(char *)newTmpBlock(4+size+size2);
  ((int*)block)[0]=size+size2;

  memcpy(block+4, func1, size);
  memcpy(block+size+4,(void*)myfunc,size2);
  if (preProc) preProc(block+size+4,size2,ctx->userfunc_data);

  ctx->computTableTop++;

  return ((int)(block));
}

static char *preprocessCode(compileContext *ctx, char *expression)
{
  int len=0;
  int alloc_len=strlen(expression)+1+64;
  char *buf=(char *)malloc(alloc_len);

  while (*expression)
  {
    if (len > alloc_len-32)
    {
      alloc_len = len+128;
      buf=(char*)realloc(buf,alloc_len);
    }

    if (expression[0] == '/')
    {
      if (expression[1] == '/')
      {
        expression+=2;
        while (expression[0] && expression[0] != '\r' && expression[0] != '\n') expression++;
      }
      else if (expression[1] == '*')
      {
        expression+=2;
        while (expression[0] && (expression[0] != '*' || expression[1] != '/')) expression++;
        if (expression[0]) expression+=2; // at this point we KNOW expression[0]=* and expression[1]=/
      }
      else 
      {
        char c=buf[len++]=*expression++;
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') ctx->l_stats[0]++;
      }
    }
    else if (expression[0] == '$')
    {
      if (toupper(expression[1]) == 'P' && toupper(expression[2]) == 'I')
      {
        static char *str="3.141592653589793";
        expression+=3;
        memcpy(buf+len,str,17);
        len+=17; //strlen(str);
        ctx->l_stats[0]+=17;
      }
      else if (toupper(expression[1]) == 'E')
      {
        static char *str="2.71828183";
        expression+=2;
        memcpy(buf+len,str,10);
        len+=10; //strlen(str);
        ctx->l_stats[0]+=10;
      }
      if (toupper(expression[1]) == 'P' && toupper(expression[2]) == 'H' && toupper(expression[3]) == 'I')
      {
        static char *str="1.61803399";
        expression+=4;
        memcpy(buf+len,str,10);
        len+=10; //strlen(str);
        ctx->l_stats[0]+=10;
      }
      else 
      {
        char c = buf[len++]=*expression++;
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') ctx->l_stats[0]++;
      }
    }
    else
    {
      char c=*expression++;
      if (c == '\r' || c == '\n' || c == '\t') c=' ';
      buf[len++]=c;
      if (c != ' ') ctx->l_stats[0]++;
    }
  }
  buf[len]=0;

  return buf;
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

//------------------------------------------------------------------------------
NSEEL_CODEHANDLE NSEEL_code_compile(NSEEL_VMCTX _ctx, char *_expression)
{
  compileContext *ctx = (compileContext *)_ctx;
  char *expression,*expression_start;
  int computable_size=0;
  codeHandleType *handle;
  startPtr *scode=NULL;
  startPtr *startpts=NULL;

  if (!ctx || !_expression || !*_expression) return 0;

  ctx->last_error_string[0]=0;
  ctx->blocks_head=0;
  ctx->tmpblocks_head=0;
  memset(ctx->l_stats,0,sizeof(ctx->l_stats));

  handle = (codeHandleType*)newBlock(sizeof(codeHandleType));

  if (!handle) 
  {
    return 0;
  }
  
  memset(handle,0,sizeof(codeHandleType));

  expression_start=expression=preprocessCode(ctx,_expression);

  while (*expression)
	{
    startPtr *tmp;
    char *expr;
    ctx->colCount=0;

    // single out segment
    while (*expression == ';' || *expression == ' ') expression++;
    if (!*expression) break;
    expr=expression;
	  while (*expression && *expression != ';') expression++;
    if (*expression) *expression++ = 0;

    // parse
    tmp=(startPtr*) newTmpBlock(sizeof(startPtr));
    if (!tmp) break;
    ctx->computTableTop=0;
    tmp->startptr=nseel_compileExpression(ctx,expr);

    if (ctx->computTableTop > NSEEL_MAX_TEMPSPACE_ENTRIES- /* safety */ 16 - /* alignment */4 ||
        !tmp->startptr) 
    { 
      lstrcpyn(ctx->last_error_string,expr,sizeof(ctx->last_error_string));
      scode=NULL; 
      break; 
    }
    if (computable_size < ctx->computTableTop)
    {
      computable_size=ctx->computTableTop;
    }

    tmp->next=NULL;
    if (!scode) scode=startpts=tmp;
    else
    {
      scode->next=tmp;
      scode=tmp;
    }
  }

  // check to see if failed on the first startingCode
  if (!scode)
  {
    freeBlocks((llBlock *)ctx->blocks_head);  // free blocks
    handle=NULL;              // return NULL (after resetting blocks_head)
  }
  else 
  {
    // now we build one big code segment out of our list of them, inserting a mov esi, computable before each item
    unsigned char *writeptr;
    int size=1; // for ret at end :)
    startPtr *p;
    p=startpts;
    while (p)
    {
      size+=2; // mov esi, edi
      size+=*(int *)p->startptr;
      p=p->next;
    }
    handle->code = newBlock(size);
    if (handle->code)
    {
      writeptr=(unsigned char *)handle->code;
      p=startpts;
      while (p)
      {
        int thissize=*(int *)p->startptr;
        *(unsigned short *)writeptr= X86_MOV_ESI_EDI;
        writeptr+=2;
        memcpy(writeptr,(char*)p->startptr + 4,thissize);
        writeptr += thissize;
      
        p=p->next;
      }
      *writeptr=X86_RET; // ret
      ctx->l_stats[1]=size;
    }
    handle->blocks = ctx->blocks_head;
    handle->workTablePtr_size=(computable_size) * sizeof(double);
  }
  freeBlocks((llBlock *)ctx->tmpblocks_head);  // free blocks
  ctx->tmpblocks_head=0;

  ctx->blocks_head=0;

  if (handle)
  {
    memcpy(handle->code_stats,ctx->l_stats,sizeof(ctx->l_stats));
    nseel_evallib_stats[0]+=ctx->l_stats[0];
    nseel_evallib_stats[1]+=ctx->l_stats[1];
    nseel_evallib_stats[2]+=ctx->l_stats[2];
    nseel_evallib_stats[3]+=ctx->l_stats[3];
    nseel_evallib_stats[4]++;
  }
  memset(ctx->l_stats,0,sizeof(ctx->l_stats));

  free(expression_start);

  return (NSEEL_CODEHANDLE)handle;
}

//------------------------------------------------------------------------------
void NSEEL_code_execute(NSEEL_CODEHANDLE code)
{
#ifdef NSEEL_REENTRANT_EXECUTION
  int baseptr;
#else
  static double _tab[NSEEL_MAX_TEMPSPACE_ENTRIES];
  int baseptr = (int) _tab;
#endif
  codeHandleType *h = (codeHandleType *)code;
  if (!h || !h->code) return;
#ifdef NSEEL_REENTRANT_EXECUTION
  baseptr = (int) alloca(h->workTablePtr_size + 16*sizeof(double) /*safety*/ + 32 /*alignment*/);
  if (!baseptr) return;
#endif

  {
    int startPoint=(int)h->code;
    __asm 
    {
      mov ebx, baseptr
      mov eax, startPoint
      pushad // Lets cover our ass
      add ebx, 31
      and ebx, ~31
      mov edi, ebx
      call eax
      popad
    }
  }
}

char *NSEEL_code_getcodeerror(NSEEL_VMCTX ctx)
{
  compileContext *c=(compileContext *)ctx;
  if (ctx && c->last_error_string[0]) return c->last_error_string;
  return 0;
}

//------------------------------------------------------------------------------
void NSEEL_code_free(NSEEL_CODEHANDLE code)
{
  codeHandleType *h = (codeHandleType *)code;
  if (h != NULL)
  {
    nseel_evallib_stats[0]-=h->code_stats[0];
    nseel_evallib_stats[1]-=h->code_stats[1];
    nseel_evallib_stats[2]-=h->code_stats[2];
    nseel_evallib_stats[3]-=h->code_stats[3];
    nseel_evallib_stats[4]--;
    freeBlocks(h->blocks);
  }
}


//------------------------------------------------------------------------------
void NSEEL_VM_resetvars(NSEEL_VMCTX _ctx)
{
  if (_ctx)
  {
    compileContext *ctx=(compileContext *)_ctx;
    int x;
    if (ctx->varTable_Names || ctx->varTable_Values) for (x = 0; x < ctx->varTable_numBlocks; x ++)
    {
      if (ctx->varTable_Names) free(ctx->varTable_Names[x]);
      if (ctx->varTable_Values) free(ctx->varTable_Values[x]);
    }

    free(ctx->varTable_Values);
    free(ctx->varTable_Names);
    ctx->varTable_Values=0;
    ctx->varTable_Names=0;

    ctx->varTable_numBlocks=0;
  }
}


NSEEL_VMCTX NSEEL_VM_alloc() // return a handle
{
  compileContext *ctx=calloc(1,sizeof(compileContext));
  return ctx;
}

void NSEEL_VM_free(NSEEL_VMCTX ctx) // free when done with a VM and ALL of its code have been freed, as well
{
  free(ctx);
}

int *NSEEL_code_getstats(NSEEL_CODEHANDLE code)
{
  codeHandleType *h = (codeHandleType *)code;
  if (h)
  {
    return h->code_stats;
  }
  return 0;
}