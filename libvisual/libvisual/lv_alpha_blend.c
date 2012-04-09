#include <config.h>
#include "lv_alpha_blend.h"
#include "lv_common.h"
#include "lv_cpu.h"

#pragma pack(1)

typedef struct {
#if VISUAL_LITTLE_ENDIAN == 1
	uint16_t b:5, g:6, r:5;
#else
	uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

static void alpha_blend_8_c	 (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);
static void alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);
static void alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);
static void alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static void alpha_blend_8_mmx  (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);
static void alpha_blend_32_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);
#endif

VisAlphaBlendFunc visual_alpha_blend_8	= alpha_blend_8_c;
VisAlphaBlendFunc visual_alpha_blend_16 = alpha_blend_16_c;
VisAlphaBlendFunc visual_alpha_blend_24 = alpha_blend_24_c;
VisAlphaBlendFunc visual_alpha_blend_32 = alpha_blend_32_c;

void visual_alpha_blend_initialize (void)
{
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	if (visual_cpu_has_mmx ()) {
		visual_alpha_blend_8  = alpha_blend_8_mmx;
		visual_alpha_blend_32 = alpha_blend_32_mmx;
	}
#endif
}

static void alpha_blend_8_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	visual_size_t i;
	for (i = 0; i < size; i++) {
		dest[i] = (alpha * (src2[i] - src1[i])) / 255 + src1[i];
	}
}

static void alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	rgb16_t *destr = (rgb16_t *) dest;
	rgb16_t *src1r = (rgb16_t *) src1;
	rgb16_t *src2r = (rgb16_t *) src2;
	visual_size_t i;

	for (i = 0; i < size / 2; i++) {
		destr[i].r = (alpha * (src2r[i].r - src1r[i].r)) / 255 + src1r[i].r;
		destr[i].g = (alpha * (src2r[i].g - src1r[i].g)) / 255 + src1r[i].g;
		destr[i].b = (alpha * (src2r[i].b - src1r[i].b)) / 255 + src1r[i].b;
	}
}

static void alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	visual_size_t i;

	for (i = 0; i < size; i++) {
		dest[i] = (alpha * (src2[i] - src1[i])) / 255 + src1[i];
	}
}

static void alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	visual_size_t i;

	for (i = 0; i < size; i++) {
		dest[i] = (alpha * (src2[i] - src1[i])) / 255 + src1[i];
	}
}

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

static void alpha_blend_8_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	visual_size_t i;
	__asm __volatile
		("\n\t pxor %%mm6, %%mm6"
		 ::);

	for (i = size; i > 4; i -= 4) {
		__asm __volatile
			("\n\t movd %[alpha], %%mm3"
			 "\n\t movd %[src2], %%mm0"
			 "\n\t psllq $32, %%mm3"
			 "\n\t movd %[alpha], %%mm2"
			 "\n\t movd %[src1], %%mm1"
			 "\n\t por %%mm3, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm0"	/* interleaving dest */
			 "\n\t punpcklbw %%mm6, %%mm1"	/* interleaving source */
			 "\n\t psubsw %%mm1, %%mm0"		/* (src - dest) part */
			 "\n\t pmullw %%mm2, %%mm0"		/* alpha * (src - dest) */
			 "\n\t psrlw $8, %%mm0"			/* / 256 */
			 "\n\t paddb %%mm1, %%mm0"		/* + dest */
			 "\n\t packuswb %%mm0, %%mm0"
			 "\n\t movd %%mm0, %[dest]"
			 : [dest] "=m" (*(dest + i))
			 : [src1] "m" (*(src1 + i))
			 , [src2] "m" (*(src2 + i))
			 , [alpha] "m" (alpha));
	}

	while (i--)
		dest[i] = (alpha * (src2[i] - src1[i])) / 255 + src1[i];

	__asm __volatile
		("\n\t emms");
}

static void alpha_blend_32_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha)
{
	visual_size_t i;

	__asm __volatile
		("\n\t pxor %%mm6, %%mm6"
		 ::);

	for (i = 0; i < size; i += 4) {
		__asm __volatile
			("\n\t movd %[src2], %%mm0"
			 "\n\t movd %[alpha], %%mm2"
			 "\n\t movd %[src1], %%mm1"
			 "\n\t psllq $32, %%mm2"
			 "\n\t movd %[alpha], %%mm3"
			 "\n\t movd %[alpha], %%mm4"
			 "\n\t psllq $16, %%mm3"
			 "\n\t por %%mm4, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm0"  /* interleaving dest */
			 "\n\t por %%mm3, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm1"  /* interleaving source */
			 "\n\t psubsw %%mm1, %%mm0"     /* (src - dest) part */
			 "\n\t pmullw %%mm2, %%mm0"     /* alpha * (src - dest) */
			 "\n\t psrlw $8, %%mm0"         /* / 256 */
			 "\n\t paddb %%mm1, %%mm0"      /* + dest */
			 "\n\t packuswb %%mm0, %%mm0"
			 "\n\t movd %%mm0, %[dest]"
			 : [dest] "=m" (*(dest + i))
			 : [src1] "m" (*(src1 + i))
			 , [src2] "m" (*(src2 + i))
			 , [alpha] "m" (alpha));
	}

	__asm __volatile
		("\n\t emms");
}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
