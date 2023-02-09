/*
Copyright (C) 2003 Antony Communier, Benjamin Kuzbyt et Daubigny Thomas

Cette biblioth�que est libre, vous pouvez la redistribuer et/ou la
modifier selon les termes de la Licence Publique G�n�rale GNU Limit�e
publi�e par la Free Software Foundation (version 2 ou bien toute autre
version ult�rieure choisie par vous).

Cette biblioth�que est distribu�e car potentiellement utile, mais SANS
AUCUNE GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but
sp�cifique. Reportez-vous � la Licence Publique G�n�rale GNU Limit�e
pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU
Limit�e en m�me temps que cette biblioth�que; si ce n'est pas le cas,
�crivez � la Free Software Foundation, Inc., 59 Temple Place, Suite
330, Boston, MA 02111-1307, �tats-Unis.
*/

#ifndef FASTMATH_H
#define FASTMATH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#if (defined(__INTEL_COMPILER))
#include <math.h> //#include <mathf.h>
#else
#include <math.h>
#endif

#undef EXTERN
#ifdef _INIT_VAR_GLOBAL
#define EXTERN
#else
#define EXTERN extern
#endif

union FloatRepr {
  float f;
  uint32_t i;
};

#define FP_ABS_BITS(fp) (fp_bits(fp) & 0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (fp_bits(fp) & 0x80000000)
#define FP_ONE_BITS 0x3F800000

// r = 1/p
#define FP_INV(r, p)                                                           \
  {                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                   \
    r = *(float *)&_i;                                                         \
    r = r * (2.0f - (p)*r);                                                    \
  }

#define FP_EXP(e, p)                                                           \
  {                                                                            \
    int _i;                                                                    \
    e = -1.44269504f * (float)0x00800000 * (p);                                \
    _i = (int)e + 0x3F800000;                                                  \
    e = *(float *)&_i;                                                         \
  }

#define FP_NORM_TO_BYTE(i, p)                                                  \
  {                                                                            \
    float _n = (p) + 1.0f;                                                     \
    i = *(int *)&_n;                                                           \
    if (i >= 0x40000000)                                                       \
      i = 0xFF;                                                                \
    else if (i <= 0x3F800000)                                                  \
      i = 0;                                                                   \
    else                                                                       \
      i = ((i) >> 15) & 0xFF;                                                  \
  }

inline uint32_t fp_bits(float fp) {
  FloatRepr tmp;
  tmp.f = fp;
  return tmp.i;
}

inline uint32_t FP_NORM_TO_BYTE2(float p) {
  FloatRepr tmp;
  tmp.f = p + 1.0f;
  return (tmp.i >> 15) & 0xFF;
}

inline uint32_t FP_NORM_TO_BYTE3(float p) {
  FloatRepr tmp;
  tmp.f = p + 12582912.0f;
  return (tmp.i & 0xFF);
}

extern uint32_t fast_sqrt_table[0x10000]; // declare table of square roots

inline void build_sqrt_table() {
  unsigned int i;
  FloatRepr s;

  for (i = 0; i <= 0x7FFF; i++) {

    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127

    s.i = (i << 8) | (0x7F << 23);
    s.f = sqrtf(s.f);

    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table

    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);

    // Repeat the process, this time with an exponent of 1,
    //  stored as 128

    s.i = (i << 8) | (0x80 << 23);
    s.f = sqrtf(s.f);

    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}

// #define fastsqrt sqrtf

inline float fastsqrt(float n) {
  FloatRepr in;
  in.f = n;

  if (in.i == 0)
    return 0.0; // check for square root of 0

  FloatRepr result;
  result.i = fast_sqrt_table[(in.i >> 8) & 0xFFFF] |
             ((((in.i - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);

  return result.f;
}

inline float distanceVector(float *v1, float *v2) {
  float diff;
  float sum = 0.0f;

  for (int i = 0; i < 3; i++) {
    diff = v1[i] - v2[i];
    sum += diff * diff;
  }

  return fastsqrt(sum);
}

#undef EXTERN

#define sqrt DONT_USE_SQRT
#endif
