/*
Copyright (C) 2003 Antony Communier, Benjamin Kuzbyt et Daubigny Thomas 

Cette bibliothèque est libre, vous pouvez la redistribuer et/ou la modifier selon les termes de la Licence Publique Générale GNU Limitée publiée par la Free Software Foundation (version 2 ou bien toute autre version ultérieure choisie par vous). 

Cette bibliothèque est distribuée car potentiellement utile, mais SANS AUCUNE GARANTIE, ni explicite ni implicite, y compris les garanties de commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la Licence Publique Générale GNU Limitée pour plus de détails. 

Vous devez avoir reçu une copie de la Licence Publique Générale GNU Limitée en même temps que cette bibliothèque; si ce n'est pas le cas, écrivez à la Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, États-Unis. 
*/

#ifndef FASTMATH_H
#define FASTMATH_H

#include <stdio.h>
#include <stdlib.h>
#if (defined(__INTEL_COMPILER))
#include <math.h>//#include <mathf.h>
#else
#include <math.h>
#endif

#undef EXTERN
#ifdef _INIT_VAR_GLOBAL
#define EXTERN 
#else
#define EXTERN extern
#endif
#define DWORD int
#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000



// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
	int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
	r = *(float *)&_i;                                                       \
	r = r * (2.0f - (p) * r);                                                \
}


#define FP_EXP(e,p)                                                          \
{                                                                            \
	int _i;                                                                  \
	e = -1.44269504f * (float)0x00800000 * (p);                              \
	_i = (int)e + 0x3F800000;                                                \
	e = *(float *)&_i;                                                       \
}

#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
	float _n = (p) + 1.0f;                                                   \
	i = *(int *)&_n;                                                         \
	if (i >= 0x40000000)     i = 0xFF;                                       \
	else if (i <=0x3F800000) i = 0;                                          \
	else i = ((i) >> 15) & 0xFF;                                             \
}



inline unsigned long FP_NORM_TO_BYTE2(float p)                                                 
	{                                                                            
	float fpTmp = p + 1.0f;                                                      
	return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
	}


inline unsigned long FP_NORM_TO_BYTE3(float p)     
	{
	float ftmp = p + 12582912.0f;                                                      
	return ((*(unsigned long *)&ftmp) & 0xFF);
	}


extern unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 

typedef union FastSqrtUnion
	{
	float f;
	unsigned int i;
	} FastSqrtUnion;

inline void  build_sqrt_table()
	{
	unsigned int i;
	FastSqrtUnion s;

	for (i = 0; i <= 0x7FFF; i++)
		{

		// Build a float with the bit pattern i as mantissa
		//  and an exponent of 0, stored as 127

		s.i = (i << 8) | (0x7F << 23);
		s.f = (float)sqrtf(s.f);

		// Take the square root then strip the first 7 bits of
		//  the mantissa into the table

		fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);

		// Repeat the process, this time with an exponent of 1, 
		//  stored as 128

		s.i = (i << 8) | (0x80 << 23);
		s.f = (float)sqrtf(s.f);

		fast_sqrt_table[i] = (s.i & 0x7FFFFF);
		}
	}

//#define fastsqrt sqrtf


inline float fastsqrt(float n)
	{

	if (FP_BITS(n) == 0)
		return 0.0;                 // check for square root of 0

	FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);

	return n;
	}

inline float distanceVector(float * v1,float * v2)
{
	float diff;
	float sum = 0.0f;

	for(int i=0;i<3;i++)
	{
		diff=v1[i]-v2[i];
		sum+=diff*diff;
	}

	return fastsqrt(sum);
}

#undef EXTERN

#define sqrt DONT_USE_SQRT
#endif

