#ifndef _TRUNC_H_
#define _TRUNC_H_

/* trunc is not a standard function.  If it is available we use it but
 * otherwise we define it in terms of ceil and floor, which are
 * standard.
 */

#ifdef HAVE_CONFIG_H

#include "config.h"

#ifdef TRUNC_IN_LIB
#ifndef TRUNC_IN_HDR
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" double trunc(double);
#else
double trunc(double);
#endif
#endif
#else
#define trunc(x) (((x) > 0.0) ? floor(x) : ceil(x))
#endif

#endif

#endif

