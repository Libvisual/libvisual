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

#ifndef __NS_EELINT_H__
#define __NS_EELINT_H__

#include "ns-eel.h"
#include "ns-eel-addfuncs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FN_ASSIGN   0
#define FN_MULTIPLY 1
#define FN_DIVIDE   2
#define FN_MODULO   3
#define FN_ADD      4
#define FN_SUB      5
#define FN_AND      6
#define FN_OR       7
#define FN_UMINUS   8
#define FN_UPLUS    9

#define MATH_SIMPLE 0
#define MATH_FN     1

#define YYSTYPE int


typedef struct
{
  double **varTable_Values;
  char   **varTable_Names;
  int varTable_numBlocks;

  int errVar;
  int colCount;
  int result;
  char last_error_string[256];
  YYSTYPE yylval;
  int	yychar;			/*  the lookahead symbol		*/
  int yynerrs;			/*  number of parse errors so far       */
  char yytext[256];
  char lastVar[256];

  char    *llsave[16];             /* Look ahead buffer            */
  char    llbuf[100];             /* work buffer                          */
  char    *llp1;//   = &llbuf[0];    /* pointer to next avail. in token      */
  char    *llp2;//   = &llbuf[0];    /* pointer to end of lookahead          */
  char    *llend;//  = &llbuf[0];    /* pointer to end of token              */
  char    *llebuf;// = &llbuf[sizeof llbuf];
  int     lleof;
  int     yyline;//  = 0;

  void *tmpblocks_head,*blocks_head;
  int computTableTop; // make it abort on potential overflow =)
  int l_stats[4]; // source bytes, static code bytes, call code bytes, data bytes

  void *userfunc_data[64];
}
compileContext;

typedef struct {
      char *name;
      void *afunc;
      void *func_e;
      int nParams;
      NSEEL_PPPROC pProc;
} functionType;


extern functionType *nseel_getFunctionFromTable(int idx);

int nseel_createCompiledValue(compileContext *ctx, double value, double *addrValue);
int nseel_createCompiledFunction1(compileContext *ctx, int fntype, int fn, int code);
int nseel_createCompiledFunction2(compileContext *ctx, int fntype, int fn, int code1, int code2);
int nseel_createCompiledFunction3(compileContext *ctx, int fntype, int fn, int code1, int code2, int code3);

extern double nseel_globalregs[100];

void nseel_resetVars(compileContext *ctx);
double *nseel_getVarPtr(compileContext *ctx, char *varName);
double *nseel_registerVar(compileContext *ctx, char *varName);


// other shat



int nseel_setVar(compileContext *ctx, int varNum);
int nseel_getVar(compileContext *ctx, int varNum);
void *nseel_compileExpression(compileContext *ctx, char *txt);

#define	VALUE	258
#define	IDENTIFIER	259
#define	FUNCTION1	260
#define	FUNCTION2	261
#define	FUNCTION3	262
#define	UMINUS	263
#define	UPLUS	264

int nseel_translate(compileContext *ctx, int type);
void nseel_count(compileContext *ctx);
void nseel_setLastVar(compileContext *ctx);
int nseel_lookup(compileContext *ctx, int *typeOfObject);
int nseel_yyerror(compileContext *ctx);
int nseel_yylex(compileContext *ctx, char **exp);
int nseel_yyparse(compileContext *ctx, char *exp);
void nseel_llinit(compileContext *ctx);
int nseel_gettoken(compileContext *ctx, char *lltb, int lltbsiz);

struct  lextab {
        int     llendst;                /* Last state number            */
        char    *lldefault;             /* Default state table          */
        char    *llnext;                /* Next state table             */
        char    *llcheck;               /* Check table                  */
        int     *llbase;                /* Base table                   */
        int     llnxtmax;               /* Last in next table           */
        int     (*llmove)();            /* Move between states          */
        char     *llfinal;               /* Final state descriptions     */
        int     (*llactr)();            /* Action routine               */
        int     *lllook;                /* Look ahead vector if != NULL */
        char    *llign;                 /* Ignore char vec if != NULL   */
        char    *llbrk;                 /* Break char vec if != NULL    */
        char    *llill;                 /* Illegal char vec if != NULL  */
};
extern struct lextab nseel_lextab;



#ifdef __cplusplus
}
#endif

#endif//__NS_EELINT_H__