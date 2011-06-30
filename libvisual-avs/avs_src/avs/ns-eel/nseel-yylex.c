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


#include "ns-eel-int.h"


#define NBPW		 16
#define EOF			(-1)


#define ERROR   256     /* yacc's value */

static int llset(compileContext *ctx);
static int llinp(compileContext *ctx, char **exp);
static int lexgetc(char **exp)
{
  char c= **exp;
  if (c) (*exp)++;
  return( c != 0 ? c : -1);
}
static int tst__b(register int c, char tab[])
{
  return (tab[(c >> 3) & 037] & (1 << (c & 07)) );
}

int nseel_gettoken(compileContext *ctx, char *lltb, int lltbsiz)
{
        register char *lp, *tp, *ep;

        tp = lltb;
        ep = tp+lltbsiz-1;
        for (lp = ctx->llbuf; lp < ctx->llend && tp < ep;)
                *tp++ = *lp++;
        *tp = 0;
        return(tp-lltb);
}


int nseel_yylex(compileContext *ctx, char **exp)
{
  register int c, st;
  int final, l, llk, i;
  register struct lextab *lp;
  char *cp;

  while (1)
  {
    llk = 0;
    if (llset(ctx)) return(0);
    st = 0;
    final = -1;
    lp = &nseel_lextab;

    do {
            if (lp->lllook && (l = lp->lllook[st])) {
                    for (c=0; c<NBPW; c++)
                            if (l&(1<<c))
                                    ctx->llsave[c] = ctx->llp1;
                    llk++;
            }
            if ((i = lp->llfinal[st]) != -1) {
                    final = i;
                    ctx->llend = ctx->llp1;
            }
            if ((c = llinp(ctx,exp)) < 0)
                    break;
            if ((cp = lp->llbrk) && llk==0 && tst__b(c, cp)) {
                    ctx->llp1--;
                    break;
            }
    } while ((st = (*lp->llmove)(lp, c, st)) != -1);


    if (ctx->llp2 < ctx->llp1)
            ctx->llp2 = ctx->llp1;
    if (final == -1) {
            ctx->llend = ctx->llp1;
            if (st == 0 && c < 0)
                    return(0);
            if ((cp = lp->llill) && tst__b(c, cp)) {
                    continue;
            }
            return(ERROR);
    }
    if (c = (final >> 11) & 037)
            ctx->llend = ctx->llsave[c-1];
    if ((c = (*lp->llactr)(ctx,final&03777)) >= 0)
            return(c);
  }
}

void nseel_llinit(compileContext *ctx)
{
   ctx->llp1 = ctx->llp2 = ctx->llend = ctx->llbuf;
   ctx->llebuf = ctx->llbuf + sizeof(ctx->llbuf);
   ctx->lleof = ctx->yyline = 0;
}


static int llinp(compileContext *ctx, char **exp)
{
        register c;
        register struct lextab *lp;
        register char *cp;

        lp = &nseel_lextab;
        cp = lp->llign;                         /* Ignore class         */
        for (;;) {
                /*
                 * Get the next character from the save buffer (if possible)
                 * If the save buffer's empty, then return EOF or the next
                 * input character.  Ignore the character if it's in the
                 * ignore class.
                 */
                c = (ctx->llp1 < ctx->llp2) ? *ctx->llp1 & 0377 : (ctx->lleof) ? EOF : lexgetc(exp);
                if (c >= 0) {                   /* Got a character?     */
                        if (cp && tst__b(c, cp))
                                continue;       /* Ignore it            */
                        if (ctx->llp1 >= ctx->llebuf) {   /* No, is there room?   */
                                return -1;
                        }
                        *ctx->llp1++ = c;            /* Store in token buff  */
                } else
                        ctx->lleof = 1;              /* Set EOF signal       */
                return(c);
        }
}

static int llset(compileContext *ctx)
/*
 * Return TRUE if EOF and nothing was moved in the look-ahead buffer
 */
{
        register char *lp1, *lp2;

        for (lp1 = ctx->llbuf, lp2 = ctx->llend; lp2 < ctx->llp2;)
                *lp1++ = *lp2++;
        ctx->llend = ctx->llp1 = ctx->llbuf;
        ctx->llp2 = lp1;
        return(ctx->lleof && lp1 == ctx->llbuf);
}
