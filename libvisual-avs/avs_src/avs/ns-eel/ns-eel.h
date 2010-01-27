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

#ifndef __NS_EEL_H__
#define __NS_EEL_H__

#ifdef __cplusplus
extern "C" {
#endif

int NSEEL_init(); // returns 0 on success
#define NSEEL_addfunction(name,nparms,code,len) NSEEL_addfunctionex((name),(nparms),(code),(len),0)
void NSEEL_addfunctionex(char *name, int nparms, int code_startaddr, int code_len, void *pproc);
void NSEEL_quit();
int *NSEEL_getstats(); // returns a pointer to 5 ints... source bytes, static code bytes, call code bytes, data bytes, number of code handles
double *NSEEL_getglobalregs();

typedef void *NSEEL_VMCTX;
typedef void *NSEEL_CODEHANDLE;

NSEEL_VMCTX NSEEL_VM_alloc(); // return a handle
void NSEEL_VM_free(NSEEL_VMCTX ctx); // free when done with a VM and ALL of its code have been freed, as well

void NSEEL_VM_resetvars(NSEEL_VMCTX ctx);
double *NSEEL_VM_regvar(NSEEL_VMCTX ctx, char *name);

NSEEL_CODEHANDLE NSEEL_code_compile(NSEEL_VMCTX ctx, char *code);
char *NSEEL_code_getcodeerror(NSEEL_VMCTX ctx);
void NSEEL_code_execute(NSEEL_CODEHANDLE code);
void NSEEL_code_free(NSEEL_CODEHANDLE code);
int *NSEEL_code_getstats(NSEEL_CODEHANDLE code); // 4 ints...source bytes, static code bytes, call code bytes, data bytes
  


// configuration:


//#define NSEEL_REENTRANT_EXECUTION
// defining this allows code to run in different threads at the same time
// this can be slower at times.

//#define NSEEL_MAX_VARIABLE_NAMELEN 8
// define this to override the max variable length (default is 8 bytes)

//#define NSEEL_MAX_TEMPSPACE_ENTRIES 2048
// define this to override the maximum working space in 8 byte units.
// 2048 is the default, and is way more than enough for most applications


//#define NSEEL_LOOPFUNC_SUPPORT
//#define NSEEL_LOOPFUNC_SUPPORT_MAXLEN (4096)
// define this for loop() support
// and define maxlen if you wish to override the maximum loop length (default is 4096 times)

#ifdef __cplusplus
}
#endif

#endif//__NS_EEL_H__