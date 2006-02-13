#ifndef _AVS_X86_OPCODE_TABLE_H
#define _AVS_X86_OPCODE_TABLE_H

#define NT0(p, e)\
	p = e,
#define NT1(p, e)\
	p##_register_xmm0_immediate = e,\
	p##_register_xmm1_immediate = e,\
	p##_register_xmm2_immediate = e,\
	p##_register_xmm3_immediate = e,\
	p##_register_xmm4_immediate = e,\
	p##_register_xmm5_immediate = e,\
	p##_register_xmm6_immediate = e,\
	p##_register_xmm7_immediate = e,\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT2(p, e)\
	p##_register_mm0_immediate = e,\
	p##_register_mm1_immediate = e,\
	p##_register_mm2_immediate = e,\
	p##_register_mm3_immediate = e,\
	p##_register_mm4_immediate = e,\
	p##_register_mm5_immediate = e,\
	p##_register_mm6_immediate = e,\
	p##_register_mm7_immediate = e,
#define NT3(p, e)\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT4(p, e)\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT5(p, e)\
	p##_register_ax = e,\
	p##_register_cx = e,\
	p##_register_dx = e,\
	p##_register_bx = e,\
	p##_register_sp = e,\
	p##_register_bp = e,\
	p##_register_si = e,\
	p##_register_di = e,\
	p##_offset = e,\
	p##_scale = e,
#define NT6(p, e)\
	p##_register_eax = e,\
	p##_register_ecx = e,\
	p##_register_edx = e,\
	p##_register_ebx = e,\
	p##_register_esp = e,\
	p##_register_ebp = e,\
	p##_register_esi = e,\
	p##_register_edi = e,\
	p##_offset = e,\
	p##_scale = e,
#define NT7(p, e)\
	p##_register_eax_register_cl = e,\
	p##_register_ecx_register_cl = e,\
	p##_register_edx_register_cl = e,\
	p##_register_ebx_register_cl = e,\
	p##_register_esp_register_cl = e,\
	p##_register_ebp_register_cl = e,\
	p##_register_esi_register_cl = e,\
	p##_register_edi_register_cl = e,\
	p##_offset_register_cl = e,\
	p##_scale_register_cl = e,
#define NT8(p, e)\
	p##_register_al_register_cl = e,\
	p##_register_cl_register_cl = e,\
	p##_register_dl_register_cl = e,\
	p##_register_bl_register_cl = e,\
	p##_register_ah_register_cl = e,\
	p##_register_ch_register_cl = e,\
	p##_register_dh_register_cl = e,\
	p##_register_bh_register_cl = e,\
	p##_offset_register_cl = e,\
	p##_scale_register_cl = e,
#define NT9(p, e)\
	p##_register_eax_register_1 = e,\
	p##_register_ecx_register_1 = e,\
	p##_register_edx_register_1 = e,\
	p##_register_ebx_register_1 = e,\
	p##_register_esp_register_1 = e,\
	p##_register_ebp_register_1 = e,\
	p##_register_esi_register_1 = e,\
	p##_register_edi_register_1 = e,\
	p##_offset_register_1 = e,\
	p##_scale_register_1 = e,
#define NT10(p, e)\
	p##_register_al_register_1 = e,\
	p##_register_cl_register_1 = e,\
	p##_register_dl_register_1 = e,\
	p##_register_bl_register_1 = e,\
	p##_register_ah_register_1 = e,\
	p##_register_ch_register_1 = e,\
	p##_register_dh_register_1 = e,\
	p##_register_bh_register_1 = e,\
	p##_offset_register_1 = e,\
	p##_scale_register_1 = e,
#define NT11(p, e)\
	p##_register_al_immediate = e,\
	p##_register_cl_immediate = e,\
	p##_register_dl_immediate = e,\
	p##_register_bl_immediate = e,\
	p##_register_ah_immediate = e,\
	p##_register_ch_immediate = e,\
	p##_register_dh_immediate = e,\
	p##_register_bh_immediate = e,\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT12(p, e)\
	p##_register_eax_immediate = e,\
	p##_register_ecx_immediate = e,\
	p##_register_edx_immediate = e,\
	p##_register_ebx_immediate = e,\
	p##_register_esp_immediate = e,\
	p##_register_ebp_immediate = e,\
	p##_register_esi_immediate = e,\
	p##_register_edi_immediate = e,\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT13(p, e)\
	p##_register_ecx_immediate = e,\
	p##_register_edx_immediate = e,\
	p##_register_ebx_immediate = e,\
	p##_register_esp_immediate = e,\
	p##_register_ebp_immediate = e,\
	p##_register_esi_immediate = e,\
	p##_register_edi_immediate = e,\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT14(p, e)\
	p##_register_cl_immediate = e,\
	p##_register_dl_immediate = e,\
	p##_register_bl_immediate = e,\
	p##_register_ah_immediate = e,\
	p##_register_ch_immediate = e,\
	p##_register_dh_immediate = e,\
	p##_register_bh_immediate = e,\
	p##_offset_immediate = e,\
	p##_scale_immediate = e,
#define NT15(p, e)\
	p##_register_ax = e,
#define NT16(p, e)\
	p##_register_st7 = e,
#define NT17(p, e)\
	p##_register_st6 = e,
#define NT18(p, e)\
	p##_register_st5 = e,
#define NT19(p, e)\
	p##_register_st4 = e,
#define NT20(p, e)\
	p##_register_st3 = e,
#define NT21(p, e)\
	p##_register_st2 = e,
#define NT22(p, e)\
	p##_register_st1 = e,
#define NT23(p, e)\
	p##_register_st0 = e,
#define NT24(p, e)\
	p##_register_st7_register_st0 = e,
#define NT25(p, e)\
	p##_register_st6_register_st0 = e,
#define NT26(p, e)\
	p##_register_st5_register_st0 = e,
#define NT27(p, e)\
	p##_register_st4_register_st0 = e,
#define NT28(p, e)\
	p##_register_st3_register_st0 = e,
#define NT29(p, e)\
	p##_register_st2_register_st0 = e,
#define NT30(p, e)\
	p##_register_st1_register_st0 = e,
#define NT31(p, e)\
	p##_register_st0_register_st7 = e,
#define NT32(p, e)\
	p##_register_st0_register_st6 = e,
#define NT33(p, e)\
	p##_register_st0_register_st5 = e,
#define NT34(p, e)\
	p##_register_st0_register_st4 = e,
#define NT35(p, e)\
	p##_register_st0_register_st3 = e,
#define NT36(p, e)\
	p##_register_st0_register_st2 = e,
#define NT37(p, e)\
	p##_register_st0_register_st1 = e,
#define NT38(p, e)\
	p##_register_st0_register_st0 = e,
#define NT39(p, e)\
	p##_register_mm0_register_xmm0 = e,\
	p##_register_mm0_register_xmm1 = e,\
	p##_register_mm0_register_xmm2 = e,\
	p##_register_mm0_register_xmm3 = e,\
	p##_register_mm0_register_xmm4 = e,\
	p##_register_mm0_register_xmm5 = e,\
	p##_register_mm0_register_xmm6 = e,\
	p##_register_mm0_register_xmm7 = e,\
	p##_register_mm0_offset = e,\
	p##_register_mm0_scale = e,\
	p##_register_mm1_register_xmm0 = e,\
	p##_register_mm1_register_xmm1 = e,\
	p##_register_mm1_register_xmm2 = e,\
	p##_register_mm1_register_xmm3 = e,\
	p##_register_mm1_register_xmm4 = e,\
	p##_register_mm1_register_xmm5 = e,\
	p##_register_mm1_register_xmm6 = e,\
	p##_register_mm1_register_xmm7 = e,\
	p##_register_mm1_offset = e,\
	p##_register_mm1_scale = e,\
	p##_register_mm2_register_xmm0 = e,\
	p##_register_mm2_register_xmm1 = e,\
	p##_register_mm2_register_xmm2 = e,\
	p##_register_mm2_register_xmm3 = e,\
	p##_register_mm2_register_xmm4 = e,\
	p##_register_mm2_register_xmm5 = e,\
	p##_register_mm2_register_xmm6 = e,\
	p##_register_mm2_register_xmm7 = e,\
	p##_register_mm2_offset = e,\
	p##_register_mm2_scale = e,\
	p##_register_mm3_register_xmm0 = e,\
	p##_register_mm3_register_xmm1 = e,\
	p##_register_mm3_register_xmm2 = e,\
	p##_register_mm3_register_xmm3 = e,\
	p##_register_mm3_register_xmm4 = e,\
	p##_register_mm3_register_xmm5 = e,\
	p##_register_mm3_register_xmm6 = e,\
	p##_register_mm3_register_xmm7 = e,\
	p##_register_mm3_offset = e,\
	p##_register_mm3_scale = e,\
	p##_register_mm4_register_xmm0 = e,\
	p##_register_mm4_register_xmm1 = e,\
	p##_register_mm4_register_xmm2 = e,\
	p##_register_mm4_register_xmm3 = e,\
	p##_register_mm4_register_xmm4 = e,\
	p##_register_mm4_register_xmm5 = e,\
	p##_register_mm4_register_xmm6 = e,\
	p##_register_mm4_register_xmm7 = e,\
	p##_register_mm4_offset = e,\
	p##_register_mm4_scale = e,\
	p##_register_mm5_register_xmm0 = e,\
	p##_register_mm5_register_xmm1 = e,\
	p##_register_mm5_register_xmm2 = e,\
	p##_register_mm5_register_xmm3 = e,\
	p##_register_mm5_register_xmm4 = e,\
	p##_register_mm5_register_xmm5 = e,\
	p##_register_mm5_register_xmm6 = e,\
	p##_register_mm5_register_xmm7 = e,\
	p##_register_mm5_offset = e,\
	p##_register_mm5_scale = e,\
	p##_register_mm6_register_xmm0 = e,\
	p##_register_mm6_register_xmm1 = e,\
	p##_register_mm6_register_xmm2 = e,\
	p##_register_mm6_register_xmm3 = e,\
	p##_register_mm6_register_xmm4 = e,\
	p##_register_mm6_register_xmm5 = e,\
	p##_register_mm6_register_xmm6 = e,\
	p##_register_mm6_register_xmm7 = e,\
	p##_register_mm6_offset = e,\
	p##_register_mm6_scale = e,\
	p##_register_mm7_register_xmm0 = e,\
	p##_register_mm7_register_xmm1 = e,\
	p##_register_mm7_register_xmm2 = e,\
	p##_register_mm7_register_xmm3 = e,\
	p##_register_mm7_register_xmm4 = e,\
	p##_register_mm7_register_xmm5 = e,\
	p##_register_mm7_register_xmm6 = e,\
	p##_register_mm7_register_xmm7 = e,\
	p##_register_mm7_offset = e,\
	p##_register_mm7_scale = e,
#define NT40(p, e)\
	p##_register_eax_register_xmm0 = e,\
	p##_register_eax_register_xmm1 = e,\
	p##_register_eax_register_xmm2 = e,\
	p##_register_eax_register_xmm3 = e,\
	p##_register_eax_register_xmm4 = e,\
	p##_register_eax_register_xmm5 = e,\
	p##_register_eax_register_xmm6 = e,\
	p##_register_eax_register_xmm7 = e,\
	p##_register_eax_offset = e,\
	p##_register_eax_scale = e,\
	p##_register_ecx_register_xmm0 = e,\
	p##_register_ecx_register_xmm1 = e,\
	p##_register_ecx_register_xmm2 = e,\
	p##_register_ecx_register_xmm3 = e,\
	p##_register_ecx_register_xmm4 = e,\
	p##_register_ecx_register_xmm5 = e,\
	p##_register_ecx_register_xmm6 = e,\
	p##_register_ecx_register_xmm7 = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_register_xmm0 = e,\
	p##_register_edx_register_xmm1 = e,\
	p##_register_edx_register_xmm2 = e,\
	p##_register_edx_register_xmm3 = e,\
	p##_register_edx_register_xmm4 = e,\
	p##_register_edx_register_xmm5 = e,\
	p##_register_edx_register_xmm6 = e,\
	p##_register_edx_register_xmm7 = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_register_xmm0 = e,\
	p##_register_ebx_register_xmm1 = e,\
	p##_register_ebx_register_xmm2 = e,\
	p##_register_ebx_register_xmm3 = e,\
	p##_register_ebx_register_xmm4 = e,\
	p##_register_ebx_register_xmm5 = e,\
	p##_register_ebx_register_xmm6 = e,\
	p##_register_ebx_register_xmm7 = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_register_xmm0 = e,\
	p##_register_esp_register_xmm1 = e,\
	p##_register_esp_register_xmm2 = e,\
	p##_register_esp_register_xmm3 = e,\
	p##_register_esp_register_xmm4 = e,\
	p##_register_esp_register_xmm5 = e,\
	p##_register_esp_register_xmm6 = e,\
	p##_register_esp_register_xmm7 = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_register_xmm0 = e,\
	p##_register_ebp_register_xmm1 = e,\
	p##_register_ebp_register_xmm2 = e,\
	p##_register_ebp_register_xmm3 = e,\
	p##_register_ebp_register_xmm4 = e,\
	p##_register_ebp_register_xmm5 = e,\
	p##_register_ebp_register_xmm6 = e,\
	p##_register_ebp_register_xmm7 = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_register_xmm0 = e,\
	p##_register_esi_register_xmm1 = e,\
	p##_register_esi_register_xmm2 = e,\
	p##_register_esi_register_xmm3 = e,\
	p##_register_esi_register_xmm4 = e,\
	p##_register_esi_register_xmm5 = e,\
	p##_register_esi_register_xmm6 = e,\
	p##_register_esi_register_xmm7 = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_register_xmm0 = e,\
	p##_register_edi_register_xmm1 = e,\
	p##_register_edi_register_xmm2 = e,\
	p##_register_edi_register_xmm3 = e,\
	p##_register_edi_register_xmm4 = e,\
	p##_register_edi_register_xmm5 = e,\
	p##_register_edi_register_xmm6 = e,\
	p##_register_edi_register_xmm7 = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT41(p, e)\
	p##_register_eax_register_xmm0 = e,\
	p##_register_eax_register_xmm1 = e,\
	p##_register_eax_register_xmm2 = e,\
	p##_register_eax_register_xmm3 = e,\
	p##_register_eax_register_xmm4 = e,\
	p##_register_eax_register_xmm5 = e,\
	p##_register_eax_register_xmm6 = e,\
	p##_register_eax_register_xmm7 = e,\
	p##_register_ecx_register_xmm0 = e,\
	p##_register_ecx_register_xmm1 = e,\
	p##_register_ecx_register_xmm2 = e,\
	p##_register_ecx_register_xmm3 = e,\
	p##_register_ecx_register_xmm4 = e,\
	p##_register_ecx_register_xmm5 = e,\
	p##_register_ecx_register_xmm6 = e,\
	p##_register_ecx_register_xmm7 = e,\
	p##_register_edx_register_xmm0 = e,\
	p##_register_edx_register_xmm1 = e,\
	p##_register_edx_register_xmm2 = e,\
	p##_register_edx_register_xmm3 = e,\
	p##_register_edx_register_xmm4 = e,\
	p##_register_edx_register_xmm5 = e,\
	p##_register_edx_register_xmm6 = e,\
	p##_register_edx_register_xmm7 = e,\
	p##_register_ebx_register_xmm0 = e,\
	p##_register_ebx_register_xmm1 = e,\
	p##_register_ebx_register_xmm2 = e,\
	p##_register_ebx_register_xmm3 = e,\
	p##_register_ebx_register_xmm4 = e,\
	p##_register_ebx_register_xmm5 = e,\
	p##_register_ebx_register_xmm6 = e,\
	p##_register_ebx_register_xmm7 = e,\
	p##_register_esp_register_xmm0 = e,\
	p##_register_esp_register_xmm1 = e,\
	p##_register_esp_register_xmm2 = e,\
	p##_register_esp_register_xmm3 = e,\
	p##_register_esp_register_xmm4 = e,\
	p##_register_esp_register_xmm5 = e,\
	p##_register_esp_register_xmm6 = e,\
	p##_register_esp_register_xmm7 = e,\
	p##_register_ebp_register_xmm0 = e,\
	p##_register_ebp_register_xmm1 = e,\
	p##_register_ebp_register_xmm2 = e,\
	p##_register_ebp_register_xmm3 = e,\
	p##_register_ebp_register_xmm4 = e,\
	p##_register_ebp_register_xmm5 = e,\
	p##_register_ebp_register_xmm6 = e,\
	p##_register_ebp_register_xmm7 = e,\
	p##_register_esi_register_xmm0 = e,\
	p##_register_esi_register_xmm1 = e,\
	p##_register_esi_register_xmm2 = e,\
	p##_register_esi_register_xmm3 = e,\
	p##_register_esi_register_xmm4 = e,\
	p##_register_esi_register_xmm5 = e,\
	p##_register_esi_register_xmm6 = e,\
	p##_register_esi_register_xmm7 = e,\
	p##_register_edi_register_xmm0 = e,\
	p##_register_edi_register_xmm1 = e,\
	p##_register_edi_register_xmm2 = e,\
	p##_register_edi_register_xmm3 = e,\
	p##_register_edi_register_xmm4 = e,\
	p##_register_edi_register_xmm5 = e,\
	p##_register_edi_register_xmm6 = e,\
	p##_register_edi_register_xmm7 = e,
#define NT42(p, e)\
	p##_register_eax_register_xmm0_immediate = e,\
	p##_register_eax_register_xmm1_immediate = e,\
	p##_register_eax_register_xmm2_immediate = e,\
	p##_register_eax_register_xmm3_immediate = e,\
	p##_register_eax_register_xmm4_immediate = e,\
	p##_register_eax_register_xmm5_immediate = e,\
	p##_register_eax_register_xmm6_immediate = e,\
	p##_register_eax_register_xmm7_immediate = e,\
	p##_register_ecx_register_xmm0_immediate = e,\
	p##_register_ecx_register_xmm1_immediate = e,\
	p##_register_ecx_register_xmm2_immediate = e,\
	p##_register_ecx_register_xmm3_immediate = e,\
	p##_register_ecx_register_xmm4_immediate = e,\
	p##_register_ecx_register_xmm5_immediate = e,\
	p##_register_ecx_register_xmm6_immediate = e,\
	p##_register_ecx_register_xmm7_immediate = e,\
	p##_register_edx_register_xmm0_immediate = e,\
	p##_register_edx_register_xmm1_immediate = e,\
	p##_register_edx_register_xmm2_immediate = e,\
	p##_register_edx_register_xmm3_immediate = e,\
	p##_register_edx_register_xmm4_immediate = e,\
	p##_register_edx_register_xmm5_immediate = e,\
	p##_register_edx_register_xmm6_immediate = e,\
	p##_register_edx_register_xmm7_immediate = e,\
	p##_register_ebx_register_xmm0_immediate = e,\
	p##_register_ebx_register_xmm1_immediate = e,\
	p##_register_ebx_register_xmm2_immediate = e,\
	p##_register_ebx_register_xmm3_immediate = e,\
	p##_register_ebx_register_xmm4_immediate = e,\
	p##_register_ebx_register_xmm5_immediate = e,\
	p##_register_ebx_register_xmm6_immediate = e,\
	p##_register_ebx_register_xmm7_immediate = e,\
	p##_register_esp_register_xmm0_immediate = e,\
	p##_register_esp_register_xmm1_immediate = e,\
	p##_register_esp_register_xmm2_immediate = e,\
	p##_register_esp_register_xmm3_immediate = e,\
	p##_register_esp_register_xmm4_immediate = e,\
	p##_register_esp_register_xmm5_immediate = e,\
	p##_register_esp_register_xmm6_immediate = e,\
	p##_register_esp_register_xmm7_immediate = e,\
	p##_register_ebp_register_xmm0_immediate = e,\
	p##_register_ebp_register_xmm1_immediate = e,\
	p##_register_ebp_register_xmm2_immediate = e,\
	p##_register_ebp_register_xmm3_immediate = e,\
	p##_register_ebp_register_xmm4_immediate = e,\
	p##_register_ebp_register_xmm5_immediate = e,\
	p##_register_ebp_register_xmm6_immediate = e,\
	p##_register_ebp_register_xmm7_immediate = e,\
	p##_register_esi_register_xmm0_immediate = e,\
	p##_register_esi_register_xmm1_immediate = e,\
	p##_register_esi_register_xmm2_immediate = e,\
	p##_register_esi_register_xmm3_immediate = e,\
	p##_register_esi_register_xmm4_immediate = e,\
	p##_register_esi_register_xmm5_immediate = e,\
	p##_register_esi_register_xmm6_immediate = e,\
	p##_register_esi_register_xmm7_immediate = e,\
	p##_register_edi_register_xmm0_immediate = e,\
	p##_register_edi_register_xmm1_immediate = e,\
	p##_register_edi_register_xmm2_immediate = e,\
	p##_register_edi_register_xmm3_immediate = e,\
	p##_register_edi_register_xmm4_immediate = e,\
	p##_register_edi_register_xmm5_immediate = e,\
	p##_register_edi_register_xmm6_immediate = e,\
	p##_register_edi_register_xmm7_immediate = e,
#define NT43(p, e)\
	p##_register_xmm0_register_eax_immediate = e,\
	p##_register_xmm0_register_ecx_immediate = e,\
	p##_register_xmm0_register_edx_immediate = e,\
	p##_register_xmm0_register_ebx_immediate = e,\
	p##_register_xmm0_register_esp_immediate = e,\
	p##_register_xmm0_register_ebp_immediate = e,\
	p##_register_xmm0_register_esi_immediate = e,\
	p##_register_xmm0_register_edi_immediate = e,\
	p##_register_xmm0_offset_immediate = e,\
	p##_register_xmm0_scale_immediate = e,\
	p##_register_xmm1_register_eax_immediate = e,\
	p##_register_xmm1_register_ecx_immediate = e,\
	p##_register_xmm1_register_edx_immediate = e,\
	p##_register_xmm1_register_ebx_immediate = e,\
	p##_register_xmm1_register_esp_immediate = e,\
	p##_register_xmm1_register_ebp_immediate = e,\
	p##_register_xmm1_register_esi_immediate = e,\
	p##_register_xmm1_register_edi_immediate = e,\
	p##_register_xmm1_offset_immediate = e,\
	p##_register_xmm1_scale_immediate = e,\
	p##_register_xmm2_register_eax_immediate = e,\
	p##_register_xmm2_register_ecx_immediate = e,\
	p##_register_xmm2_register_edx_immediate = e,\
	p##_register_xmm2_register_ebx_immediate = e,\
	p##_register_xmm2_register_esp_immediate = e,\
	p##_register_xmm2_register_ebp_immediate = e,\
	p##_register_xmm2_register_esi_immediate = e,\
	p##_register_xmm2_register_edi_immediate = e,\
	p##_register_xmm2_offset_immediate = e,\
	p##_register_xmm2_scale_immediate = e,\
	p##_register_xmm3_register_eax_immediate = e,\
	p##_register_xmm3_register_ecx_immediate = e,\
	p##_register_xmm3_register_edx_immediate = e,\
	p##_register_xmm3_register_ebx_immediate = e,\
	p##_register_xmm3_register_esp_immediate = e,\
	p##_register_xmm3_register_ebp_immediate = e,\
	p##_register_xmm3_register_esi_immediate = e,\
	p##_register_xmm3_register_edi_immediate = e,\
	p##_register_xmm3_offset_immediate = e,\
	p##_register_xmm3_scale_immediate = e,\
	p##_register_xmm4_register_eax_immediate = e,\
	p##_register_xmm4_register_ecx_immediate = e,\
	p##_register_xmm4_register_edx_immediate = e,\
	p##_register_xmm4_register_ebx_immediate = e,\
	p##_register_xmm4_register_esp_immediate = e,\
	p##_register_xmm4_register_ebp_immediate = e,\
	p##_register_xmm4_register_esi_immediate = e,\
	p##_register_xmm4_register_edi_immediate = e,\
	p##_register_xmm4_offset_immediate = e,\
	p##_register_xmm4_scale_immediate = e,\
	p##_register_xmm5_register_eax_immediate = e,\
	p##_register_xmm5_register_ecx_immediate = e,\
	p##_register_xmm5_register_edx_immediate = e,\
	p##_register_xmm5_register_ebx_immediate = e,\
	p##_register_xmm5_register_esp_immediate = e,\
	p##_register_xmm5_register_ebp_immediate = e,\
	p##_register_xmm5_register_esi_immediate = e,\
	p##_register_xmm5_register_edi_immediate = e,\
	p##_register_xmm5_offset_immediate = e,\
	p##_register_xmm5_scale_immediate = e,\
	p##_register_xmm6_register_eax_immediate = e,\
	p##_register_xmm6_register_ecx_immediate = e,\
	p##_register_xmm6_register_edx_immediate = e,\
	p##_register_xmm6_register_ebx_immediate = e,\
	p##_register_xmm6_register_esp_immediate = e,\
	p##_register_xmm6_register_ebp_immediate = e,\
	p##_register_xmm6_register_esi_immediate = e,\
	p##_register_xmm6_register_edi_immediate = e,\
	p##_register_xmm6_offset_immediate = e,\
	p##_register_xmm6_scale_immediate = e,\
	p##_register_xmm7_register_eax_immediate = e,\
	p##_register_xmm7_register_ecx_immediate = e,\
	p##_register_xmm7_register_edx_immediate = e,\
	p##_register_xmm7_register_ebx_immediate = e,\
	p##_register_xmm7_register_esp_immediate = e,\
	p##_register_xmm7_register_ebp_immediate = e,\
	p##_register_xmm7_register_esi_immediate = e,\
	p##_register_xmm7_register_edi_immediate = e,\
	p##_register_xmm7_offset_immediate = e,\
	p##_register_xmm7_scale_immediate = e,
#define NT44(p, e)\
	p##_register_xmm0_register_eax = e,\
	p##_register_xmm0_register_ecx = e,\
	p##_register_xmm0_register_edx = e,\
	p##_register_xmm0_register_ebx = e,\
	p##_register_xmm0_register_esp = e,\
	p##_register_xmm0_register_ebp = e,\
	p##_register_xmm0_register_esi = e,\
	p##_register_xmm0_register_edi = e,\
	p##_register_xmm0_offset = e,\
	p##_register_xmm0_scale = e,\
	p##_register_xmm1_register_eax = e,\
	p##_register_xmm1_register_ecx = e,\
	p##_register_xmm1_register_edx = e,\
	p##_register_xmm1_register_ebx = e,\
	p##_register_xmm1_register_esp = e,\
	p##_register_xmm1_register_ebp = e,\
	p##_register_xmm1_register_esi = e,\
	p##_register_xmm1_register_edi = e,\
	p##_register_xmm1_offset = e,\
	p##_register_xmm1_scale = e,\
	p##_register_xmm2_register_eax = e,\
	p##_register_xmm2_register_ecx = e,\
	p##_register_xmm2_register_edx = e,\
	p##_register_xmm2_register_ebx = e,\
	p##_register_xmm2_register_esp = e,\
	p##_register_xmm2_register_ebp = e,\
	p##_register_xmm2_register_esi = e,\
	p##_register_xmm2_register_edi = e,\
	p##_register_xmm2_offset = e,\
	p##_register_xmm2_scale = e,\
	p##_register_xmm3_register_eax = e,\
	p##_register_xmm3_register_ecx = e,\
	p##_register_xmm3_register_edx = e,\
	p##_register_xmm3_register_ebx = e,\
	p##_register_xmm3_register_esp = e,\
	p##_register_xmm3_register_ebp = e,\
	p##_register_xmm3_register_esi = e,\
	p##_register_xmm3_register_edi = e,\
	p##_register_xmm3_offset = e,\
	p##_register_xmm3_scale = e,\
	p##_register_xmm4_register_eax = e,\
	p##_register_xmm4_register_ecx = e,\
	p##_register_xmm4_register_edx = e,\
	p##_register_xmm4_register_ebx = e,\
	p##_register_xmm4_register_esp = e,\
	p##_register_xmm4_register_ebp = e,\
	p##_register_xmm4_register_esi = e,\
	p##_register_xmm4_register_edi = e,\
	p##_register_xmm4_offset = e,\
	p##_register_xmm4_scale = e,\
	p##_register_xmm5_register_eax = e,\
	p##_register_xmm5_register_ecx = e,\
	p##_register_xmm5_register_edx = e,\
	p##_register_xmm5_register_ebx = e,\
	p##_register_xmm5_register_esp = e,\
	p##_register_xmm5_register_ebp = e,\
	p##_register_xmm5_register_esi = e,\
	p##_register_xmm5_register_edi = e,\
	p##_register_xmm5_offset = e,\
	p##_register_xmm5_scale = e,\
	p##_register_xmm6_register_eax = e,\
	p##_register_xmm6_register_ecx = e,\
	p##_register_xmm6_register_edx = e,\
	p##_register_xmm6_register_ebx = e,\
	p##_register_xmm6_register_esp = e,\
	p##_register_xmm6_register_ebp = e,\
	p##_register_xmm6_register_esi = e,\
	p##_register_xmm6_register_edi = e,\
	p##_register_xmm6_offset = e,\
	p##_register_xmm6_scale = e,\
	p##_register_xmm7_register_eax = e,\
	p##_register_xmm7_register_ecx = e,\
	p##_register_xmm7_register_edx = e,\
	p##_register_xmm7_register_ebx = e,\
	p##_register_xmm7_register_esp = e,\
	p##_register_xmm7_register_ebp = e,\
	p##_register_xmm7_register_esi = e,\
	p##_register_xmm7_register_edi = e,\
	p##_register_xmm7_offset = e,\
	p##_register_xmm7_scale = e,
#define NT45(p, e)\
	p##_register_eax_register_mm0 = e,\
	p##_register_eax_register_mm1 = e,\
	p##_register_eax_register_mm2 = e,\
	p##_register_eax_register_mm3 = e,\
	p##_register_eax_register_mm4 = e,\
	p##_register_eax_register_mm5 = e,\
	p##_register_eax_register_mm6 = e,\
	p##_register_eax_register_mm7 = e,\
	p##_register_ecx_register_mm0 = e,\
	p##_register_ecx_register_mm1 = e,\
	p##_register_ecx_register_mm2 = e,\
	p##_register_ecx_register_mm3 = e,\
	p##_register_ecx_register_mm4 = e,\
	p##_register_ecx_register_mm5 = e,\
	p##_register_ecx_register_mm6 = e,\
	p##_register_ecx_register_mm7 = e,\
	p##_register_edx_register_mm0 = e,\
	p##_register_edx_register_mm1 = e,\
	p##_register_edx_register_mm2 = e,\
	p##_register_edx_register_mm3 = e,\
	p##_register_edx_register_mm4 = e,\
	p##_register_edx_register_mm5 = e,\
	p##_register_edx_register_mm6 = e,\
	p##_register_edx_register_mm7 = e,\
	p##_register_ebx_register_mm0 = e,\
	p##_register_ebx_register_mm1 = e,\
	p##_register_ebx_register_mm2 = e,\
	p##_register_ebx_register_mm3 = e,\
	p##_register_ebx_register_mm4 = e,\
	p##_register_ebx_register_mm5 = e,\
	p##_register_ebx_register_mm6 = e,\
	p##_register_ebx_register_mm7 = e,\
	p##_register_esp_register_mm0 = e,\
	p##_register_esp_register_mm1 = e,\
	p##_register_esp_register_mm2 = e,\
	p##_register_esp_register_mm3 = e,\
	p##_register_esp_register_mm4 = e,\
	p##_register_esp_register_mm5 = e,\
	p##_register_esp_register_mm6 = e,\
	p##_register_esp_register_mm7 = e,\
	p##_register_ebp_register_mm0 = e,\
	p##_register_ebp_register_mm1 = e,\
	p##_register_ebp_register_mm2 = e,\
	p##_register_ebp_register_mm3 = e,\
	p##_register_ebp_register_mm4 = e,\
	p##_register_ebp_register_mm5 = e,\
	p##_register_ebp_register_mm6 = e,\
	p##_register_ebp_register_mm7 = e,\
	p##_register_esi_register_mm0 = e,\
	p##_register_esi_register_mm1 = e,\
	p##_register_esi_register_mm2 = e,\
	p##_register_esi_register_mm3 = e,\
	p##_register_esi_register_mm4 = e,\
	p##_register_esi_register_mm5 = e,\
	p##_register_esi_register_mm6 = e,\
	p##_register_esi_register_mm7 = e,\
	p##_register_edi_register_mm0 = e,\
	p##_register_edi_register_mm1 = e,\
	p##_register_edi_register_mm2 = e,\
	p##_register_edi_register_mm3 = e,\
	p##_register_edi_register_mm4 = e,\
	p##_register_edi_register_mm5 = e,\
	p##_register_edi_register_mm6 = e,\
	p##_register_edi_register_mm7 = e,
#define NT46(p, e)\
	p##_register_edi = e,
#define NT47(p, e)\
	p##_register_esi = e,
#define NT48(p, e)\
	p##_register_ebp = e,
#define NT49(p, e)\
	p##_register_esp = e,
#define NT50(p, e)\
	p##_register_ebx = e,
#define NT51(p, e)\
	p##_register_edx = e,
#define NT52(p, e)\
	p##_register_ecx = e,
#define NT53(p, e)\
	p##_register_eax = e,
#define NT54(p, e)\
	p##_register_eax_register_mm0_immediate = e,\
	p##_register_eax_register_mm1_immediate = e,\
	p##_register_eax_register_mm2_immediate = e,\
	p##_register_eax_register_mm3_immediate = e,\
	p##_register_eax_register_mm4_immediate = e,\
	p##_register_eax_register_mm5_immediate = e,\
	p##_register_eax_register_mm6_immediate = e,\
	p##_register_eax_register_mm7_immediate = e,\
	p##_register_ecx_register_mm0_immediate = e,\
	p##_register_ecx_register_mm1_immediate = e,\
	p##_register_ecx_register_mm2_immediate = e,\
	p##_register_ecx_register_mm3_immediate = e,\
	p##_register_ecx_register_mm4_immediate = e,\
	p##_register_ecx_register_mm5_immediate = e,\
	p##_register_ecx_register_mm6_immediate = e,\
	p##_register_ecx_register_mm7_immediate = e,\
	p##_register_edx_register_mm0_immediate = e,\
	p##_register_edx_register_mm1_immediate = e,\
	p##_register_edx_register_mm2_immediate = e,\
	p##_register_edx_register_mm3_immediate = e,\
	p##_register_edx_register_mm4_immediate = e,\
	p##_register_edx_register_mm5_immediate = e,\
	p##_register_edx_register_mm6_immediate = e,\
	p##_register_edx_register_mm7_immediate = e,\
	p##_register_ebx_register_mm0_immediate = e,\
	p##_register_ebx_register_mm1_immediate = e,\
	p##_register_ebx_register_mm2_immediate = e,\
	p##_register_ebx_register_mm3_immediate = e,\
	p##_register_ebx_register_mm4_immediate = e,\
	p##_register_ebx_register_mm5_immediate = e,\
	p##_register_ebx_register_mm6_immediate = e,\
	p##_register_ebx_register_mm7_immediate = e,\
	p##_register_esp_register_mm0_immediate = e,\
	p##_register_esp_register_mm1_immediate = e,\
	p##_register_esp_register_mm2_immediate = e,\
	p##_register_esp_register_mm3_immediate = e,\
	p##_register_esp_register_mm4_immediate = e,\
	p##_register_esp_register_mm5_immediate = e,\
	p##_register_esp_register_mm6_immediate = e,\
	p##_register_esp_register_mm7_immediate = e,\
	p##_register_ebp_register_mm0_immediate = e,\
	p##_register_ebp_register_mm1_immediate = e,\
	p##_register_ebp_register_mm2_immediate = e,\
	p##_register_ebp_register_mm3_immediate = e,\
	p##_register_ebp_register_mm4_immediate = e,\
	p##_register_ebp_register_mm5_immediate = e,\
	p##_register_ebp_register_mm6_immediate = e,\
	p##_register_ebp_register_mm7_immediate = e,\
	p##_register_esi_register_mm0_immediate = e,\
	p##_register_esi_register_mm1_immediate = e,\
	p##_register_esi_register_mm2_immediate = e,\
	p##_register_esi_register_mm3_immediate = e,\
	p##_register_esi_register_mm4_immediate = e,\
	p##_register_esi_register_mm5_immediate = e,\
	p##_register_esi_register_mm6_immediate = e,\
	p##_register_esi_register_mm7_immediate = e,\
	p##_register_edi_register_mm0_immediate = e,\
	p##_register_edi_register_mm1_immediate = e,\
	p##_register_edi_register_mm2_immediate = e,\
	p##_register_edi_register_mm3_immediate = e,\
	p##_register_edi_register_mm4_immediate = e,\
	p##_register_edi_register_mm5_immediate = e,\
	p##_register_edi_register_mm6_immediate = e,\
	p##_register_edi_register_mm7_immediate = e,
#define NT55(p, e)\
	p##_register_mm0_register_eax_immediate = e,\
	p##_register_mm0_register_ecx_immediate = e,\
	p##_register_mm0_register_edx_immediate = e,\
	p##_register_mm0_register_ebx_immediate = e,\
	p##_register_mm0_register_esp_immediate = e,\
	p##_register_mm0_register_ebp_immediate = e,\
	p##_register_mm0_register_esi_immediate = e,\
	p##_register_mm0_register_edi_immediate = e,\
	p##_register_mm0_offset_immediate = e,\
	p##_register_mm0_scale_immediate = e,\
	p##_register_mm1_register_eax_immediate = e,\
	p##_register_mm1_register_ecx_immediate = e,\
	p##_register_mm1_register_edx_immediate = e,\
	p##_register_mm1_register_ebx_immediate = e,\
	p##_register_mm1_register_esp_immediate = e,\
	p##_register_mm1_register_ebp_immediate = e,\
	p##_register_mm1_register_esi_immediate = e,\
	p##_register_mm1_register_edi_immediate = e,\
	p##_register_mm1_offset_immediate = e,\
	p##_register_mm1_scale_immediate = e,\
	p##_register_mm2_register_eax_immediate = e,\
	p##_register_mm2_register_ecx_immediate = e,\
	p##_register_mm2_register_edx_immediate = e,\
	p##_register_mm2_register_ebx_immediate = e,\
	p##_register_mm2_register_esp_immediate = e,\
	p##_register_mm2_register_ebp_immediate = e,\
	p##_register_mm2_register_esi_immediate = e,\
	p##_register_mm2_register_edi_immediate = e,\
	p##_register_mm2_offset_immediate = e,\
	p##_register_mm2_scale_immediate = e,\
	p##_register_mm3_register_eax_immediate = e,\
	p##_register_mm3_register_ecx_immediate = e,\
	p##_register_mm3_register_edx_immediate = e,\
	p##_register_mm3_register_ebx_immediate = e,\
	p##_register_mm3_register_esp_immediate = e,\
	p##_register_mm3_register_ebp_immediate = e,\
	p##_register_mm3_register_esi_immediate = e,\
	p##_register_mm3_register_edi_immediate = e,\
	p##_register_mm3_offset_immediate = e,\
	p##_register_mm3_scale_immediate = e,\
	p##_register_mm4_register_eax_immediate = e,\
	p##_register_mm4_register_ecx_immediate = e,\
	p##_register_mm4_register_edx_immediate = e,\
	p##_register_mm4_register_ebx_immediate = e,\
	p##_register_mm4_register_esp_immediate = e,\
	p##_register_mm4_register_ebp_immediate = e,\
	p##_register_mm4_register_esi_immediate = e,\
	p##_register_mm4_register_edi_immediate = e,\
	p##_register_mm4_offset_immediate = e,\
	p##_register_mm4_scale_immediate = e,\
	p##_register_mm5_register_eax_immediate = e,\
	p##_register_mm5_register_ecx_immediate = e,\
	p##_register_mm5_register_edx_immediate = e,\
	p##_register_mm5_register_ebx_immediate = e,\
	p##_register_mm5_register_esp_immediate = e,\
	p##_register_mm5_register_ebp_immediate = e,\
	p##_register_mm5_register_esi_immediate = e,\
	p##_register_mm5_register_edi_immediate = e,\
	p##_register_mm5_offset_immediate = e,\
	p##_register_mm5_scale_immediate = e,\
	p##_register_mm6_register_eax_immediate = e,\
	p##_register_mm6_register_ecx_immediate = e,\
	p##_register_mm6_register_edx_immediate = e,\
	p##_register_mm6_register_ebx_immediate = e,\
	p##_register_mm6_register_esp_immediate = e,\
	p##_register_mm6_register_ebp_immediate = e,\
	p##_register_mm6_register_esi_immediate = e,\
	p##_register_mm6_register_edi_immediate = e,\
	p##_register_mm6_offset_immediate = e,\
	p##_register_mm6_scale_immediate = e,\
	p##_register_mm7_register_eax_immediate = e,\
	p##_register_mm7_register_ecx_immediate = e,\
	p##_register_mm7_register_edx_immediate = e,\
	p##_register_mm7_register_ebx_immediate = e,\
	p##_register_mm7_register_esp_immediate = e,\
	p##_register_mm7_register_ebp_immediate = e,\
	p##_register_mm7_register_esi_immediate = e,\
	p##_register_mm7_register_edi_immediate = e,\
	p##_register_mm7_offset_immediate = e,\
	p##_register_mm7_scale_immediate = e,
#define NT56(p, e)\
	p##_offset_register_eax = e,\
	p##_offset_register_ecx = e,\
	p##_offset_register_edx = e,\
	p##_offset_register_ebx = e,\
	p##_offset_register_esp = e,\
	p##_offset_register_ebp = e,\
	p##_offset_register_esi = e,\
	p##_offset_register_edi = e,\
	p##_scale_register_eax = e,\
	p##_scale_register_ecx = e,\
	p##_scale_register_edx = e,\
	p##_scale_register_ebx = e,\
	p##_scale_register_esp = e,\
	p##_scale_register_ebp = e,\
	p##_scale_register_esi = e,\
	p##_scale_register_edi = e,
#define NT57(p, e)\
	p##_register_xmm0_register_xmm0_immediate = e,\
	p##_register_xmm0_register_xmm1_immediate = e,\
	p##_register_xmm0_register_xmm2_immediate = e,\
	p##_register_xmm0_register_xmm3_immediate = e,\
	p##_register_xmm0_register_xmm4_immediate = e,\
	p##_register_xmm0_register_xmm5_immediate = e,\
	p##_register_xmm0_register_xmm6_immediate = e,\
	p##_register_xmm0_register_xmm7_immediate = e,\
	p##_register_xmm0_offset_immediate = e,\
	p##_register_xmm0_scale_immediate = e,\
	p##_register_xmm1_register_xmm0_immediate = e,\
	p##_register_xmm1_register_xmm1_immediate = e,\
	p##_register_xmm1_register_xmm2_immediate = e,\
	p##_register_xmm1_register_xmm3_immediate = e,\
	p##_register_xmm1_register_xmm4_immediate = e,\
	p##_register_xmm1_register_xmm5_immediate = e,\
	p##_register_xmm1_register_xmm6_immediate = e,\
	p##_register_xmm1_register_xmm7_immediate = e,\
	p##_register_xmm1_offset_immediate = e,\
	p##_register_xmm1_scale_immediate = e,\
	p##_register_xmm2_register_xmm0_immediate = e,\
	p##_register_xmm2_register_xmm1_immediate = e,\
	p##_register_xmm2_register_xmm2_immediate = e,\
	p##_register_xmm2_register_xmm3_immediate = e,\
	p##_register_xmm2_register_xmm4_immediate = e,\
	p##_register_xmm2_register_xmm5_immediate = e,\
	p##_register_xmm2_register_xmm6_immediate = e,\
	p##_register_xmm2_register_xmm7_immediate = e,\
	p##_register_xmm2_offset_immediate = e,\
	p##_register_xmm2_scale_immediate = e,\
	p##_register_xmm3_register_xmm0_immediate = e,\
	p##_register_xmm3_register_xmm1_immediate = e,\
	p##_register_xmm3_register_xmm2_immediate = e,\
	p##_register_xmm3_register_xmm3_immediate = e,\
	p##_register_xmm3_register_xmm4_immediate = e,\
	p##_register_xmm3_register_xmm5_immediate = e,\
	p##_register_xmm3_register_xmm6_immediate = e,\
	p##_register_xmm3_register_xmm7_immediate = e,\
	p##_register_xmm3_offset_immediate = e,\
	p##_register_xmm3_scale_immediate = e,\
	p##_register_xmm4_register_xmm0_immediate = e,\
	p##_register_xmm4_register_xmm1_immediate = e,\
	p##_register_xmm4_register_xmm2_immediate = e,\
	p##_register_xmm4_register_xmm3_immediate = e,\
	p##_register_xmm4_register_xmm4_immediate = e,\
	p##_register_xmm4_register_xmm5_immediate = e,\
	p##_register_xmm4_register_xmm6_immediate = e,\
	p##_register_xmm4_register_xmm7_immediate = e,\
	p##_register_xmm4_offset_immediate = e,\
	p##_register_xmm4_scale_immediate = e,\
	p##_register_xmm5_register_xmm0_immediate = e,\
	p##_register_xmm5_register_xmm1_immediate = e,\
	p##_register_xmm5_register_xmm2_immediate = e,\
	p##_register_xmm5_register_xmm3_immediate = e,\
	p##_register_xmm5_register_xmm4_immediate = e,\
	p##_register_xmm5_register_xmm5_immediate = e,\
	p##_register_xmm5_register_xmm6_immediate = e,\
	p##_register_xmm5_register_xmm7_immediate = e,\
	p##_register_xmm5_offset_immediate = e,\
	p##_register_xmm5_scale_immediate = e,\
	p##_register_xmm6_register_xmm0_immediate = e,\
	p##_register_xmm6_register_xmm1_immediate = e,\
	p##_register_xmm6_register_xmm2_immediate = e,\
	p##_register_xmm6_register_xmm3_immediate = e,\
	p##_register_xmm6_register_xmm4_immediate = e,\
	p##_register_xmm6_register_xmm5_immediate = e,\
	p##_register_xmm6_register_xmm6_immediate = e,\
	p##_register_xmm6_register_xmm7_immediate = e,\
	p##_register_xmm6_offset_immediate = e,\
	p##_register_xmm6_scale_immediate = e,\
	p##_register_xmm7_register_xmm0_immediate = e,\
	p##_register_xmm7_register_xmm1_immediate = e,\
	p##_register_xmm7_register_xmm2_immediate = e,\
	p##_register_xmm7_register_xmm3_immediate = e,\
	p##_register_xmm7_register_xmm4_immediate = e,\
	p##_register_xmm7_register_xmm5_immediate = e,\
	p##_register_xmm7_register_xmm6_immediate = e,\
	p##_register_xmm7_register_xmm7_immediate = e,\
	p##_register_xmm7_offset_immediate = e,\
	p##_register_xmm7_scale_immediate = e,
#define NT58(p, e)\
	p##_register_eax_register_al = e,\
	p##_register_eax_register_cl = e,\
	p##_register_eax_register_dl = e,\
	p##_register_eax_register_bl = e,\
	p##_register_eax_register_ah = e,\
	p##_register_eax_register_ch = e,\
	p##_register_eax_register_dh = e,\
	p##_register_eax_register_bh = e,\
	p##_register_eax_offset = e,\
	p##_register_eax_scale = e,\
	p##_register_ecx_register_al = e,\
	p##_register_ecx_register_cl = e,\
	p##_register_ecx_register_dl = e,\
	p##_register_ecx_register_bl = e,\
	p##_register_ecx_register_ah = e,\
	p##_register_ecx_register_ch = e,\
	p##_register_ecx_register_dh = e,\
	p##_register_ecx_register_bh = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_register_al = e,\
	p##_register_edx_register_cl = e,\
	p##_register_edx_register_dl = e,\
	p##_register_edx_register_bl = e,\
	p##_register_edx_register_ah = e,\
	p##_register_edx_register_ch = e,\
	p##_register_edx_register_dh = e,\
	p##_register_edx_register_bh = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_register_al = e,\
	p##_register_ebx_register_cl = e,\
	p##_register_ebx_register_dl = e,\
	p##_register_ebx_register_bl = e,\
	p##_register_ebx_register_ah = e,\
	p##_register_ebx_register_ch = e,\
	p##_register_ebx_register_dh = e,\
	p##_register_ebx_register_bh = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_register_al = e,\
	p##_register_esp_register_cl = e,\
	p##_register_esp_register_dl = e,\
	p##_register_esp_register_bl = e,\
	p##_register_esp_register_ah = e,\
	p##_register_esp_register_ch = e,\
	p##_register_esp_register_dh = e,\
	p##_register_esp_register_bh = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_register_al = e,\
	p##_register_ebp_register_cl = e,\
	p##_register_ebp_register_dl = e,\
	p##_register_ebp_register_bl = e,\
	p##_register_ebp_register_ah = e,\
	p##_register_ebp_register_ch = e,\
	p##_register_ebp_register_dh = e,\
	p##_register_ebp_register_bh = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_register_al = e,\
	p##_register_esi_register_cl = e,\
	p##_register_esi_register_dl = e,\
	p##_register_esi_register_bl = e,\
	p##_register_esi_register_ah = e,\
	p##_register_esi_register_ch = e,\
	p##_register_esi_register_dh = e,\
	p##_register_esi_register_bh = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_register_al = e,\
	p##_register_edi_register_cl = e,\
	p##_register_edi_register_dl = e,\
	p##_register_edi_register_bl = e,\
	p##_register_edi_register_ah = e,\
	p##_register_edi_register_ch = e,\
	p##_register_edi_register_dh = e,\
	p##_register_edi_register_bh = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT59(p, e)\
	p##_offset = e,\
	p##_scale = e,
#define NT60(p, e)\
	p##_register_gs = e,
#define NT61(p, e)\
	p##_register_eax_register_eax_register_cl = e,\
	p##_register_eax_register_ecx_register_cl = e,\
	p##_register_eax_register_edx_register_cl = e,\
	p##_register_eax_register_ebx_register_cl = e,\
	p##_register_eax_register_esp_register_cl = e,\
	p##_register_eax_register_ebp_register_cl = e,\
	p##_register_eax_register_esi_register_cl = e,\
	p##_register_eax_register_edi_register_cl = e,\
	p##_register_ecx_register_eax_register_cl = e,\
	p##_register_ecx_register_ecx_register_cl = e,\
	p##_register_ecx_register_edx_register_cl = e,\
	p##_register_ecx_register_ebx_register_cl = e,\
	p##_register_ecx_register_esp_register_cl = e,\
	p##_register_ecx_register_ebp_register_cl = e,\
	p##_register_ecx_register_esi_register_cl = e,\
	p##_register_ecx_register_edi_register_cl = e,\
	p##_register_edx_register_eax_register_cl = e,\
	p##_register_edx_register_ecx_register_cl = e,\
	p##_register_edx_register_edx_register_cl = e,\
	p##_register_edx_register_ebx_register_cl = e,\
	p##_register_edx_register_esp_register_cl = e,\
	p##_register_edx_register_ebp_register_cl = e,\
	p##_register_edx_register_esi_register_cl = e,\
	p##_register_edx_register_edi_register_cl = e,\
	p##_register_ebx_register_eax_register_cl = e,\
	p##_register_ebx_register_ecx_register_cl = e,\
	p##_register_ebx_register_edx_register_cl = e,\
	p##_register_ebx_register_ebx_register_cl = e,\
	p##_register_ebx_register_esp_register_cl = e,\
	p##_register_ebx_register_ebp_register_cl = e,\
	p##_register_ebx_register_esi_register_cl = e,\
	p##_register_ebx_register_edi_register_cl = e,\
	p##_register_esp_register_eax_register_cl = e,\
	p##_register_esp_register_ecx_register_cl = e,\
	p##_register_esp_register_edx_register_cl = e,\
	p##_register_esp_register_ebx_register_cl = e,\
	p##_register_esp_register_esp_register_cl = e,\
	p##_register_esp_register_ebp_register_cl = e,\
	p##_register_esp_register_esi_register_cl = e,\
	p##_register_esp_register_edi_register_cl = e,\
	p##_register_ebp_register_eax_register_cl = e,\
	p##_register_ebp_register_ecx_register_cl = e,\
	p##_register_ebp_register_edx_register_cl = e,\
	p##_register_ebp_register_ebx_register_cl = e,\
	p##_register_ebp_register_esp_register_cl = e,\
	p##_register_ebp_register_ebp_register_cl = e,\
	p##_register_ebp_register_esi_register_cl = e,\
	p##_register_ebp_register_edi_register_cl = e,\
	p##_register_esi_register_eax_register_cl = e,\
	p##_register_esi_register_ecx_register_cl = e,\
	p##_register_esi_register_edx_register_cl = e,\
	p##_register_esi_register_ebx_register_cl = e,\
	p##_register_esi_register_esp_register_cl = e,\
	p##_register_esi_register_ebp_register_cl = e,\
	p##_register_esi_register_esi_register_cl = e,\
	p##_register_esi_register_edi_register_cl = e,\
	p##_register_edi_register_eax_register_cl = e,\
	p##_register_edi_register_ecx_register_cl = e,\
	p##_register_edi_register_edx_register_cl = e,\
	p##_register_edi_register_ebx_register_cl = e,\
	p##_register_edi_register_esp_register_cl = e,\
	p##_register_edi_register_ebp_register_cl = e,\
	p##_register_edi_register_esi_register_cl = e,\
	p##_register_edi_register_edi_register_cl = e,\
	p##_offset_register_eax_register_cl = e,\
	p##_offset_register_ecx_register_cl = e,\
	p##_offset_register_edx_register_cl = e,\
	p##_offset_register_ebx_register_cl = e,\
	p##_offset_register_esp_register_cl = e,\
	p##_offset_register_ebp_register_cl = e,\
	p##_offset_register_esi_register_cl = e,\
	p##_offset_register_edi_register_cl = e,\
	p##_scale_register_eax_register_cl = e,\
	p##_scale_register_ecx_register_cl = e,\
	p##_scale_register_edx_register_cl = e,\
	p##_scale_register_ebx_register_cl = e,\
	p##_scale_register_esp_register_cl = e,\
	p##_scale_register_ebp_register_cl = e,\
	p##_scale_register_esi_register_cl = e,\
	p##_scale_register_edi_register_cl = e,
#define NT62(p, e)\
	p##_register_eax_register_eax_immediate = e,\
	p##_register_eax_register_ecx_immediate = e,\
	p##_register_eax_register_edx_immediate = e,\
	p##_register_eax_register_ebx_immediate = e,\
	p##_register_eax_register_esp_immediate = e,\
	p##_register_eax_register_ebp_immediate = e,\
	p##_register_eax_register_esi_immediate = e,\
	p##_register_eax_register_edi_immediate = e,\
	p##_register_ecx_register_eax_immediate = e,\
	p##_register_ecx_register_ecx_immediate = e,\
	p##_register_ecx_register_edx_immediate = e,\
	p##_register_ecx_register_ebx_immediate = e,\
	p##_register_ecx_register_esp_immediate = e,\
	p##_register_ecx_register_ebp_immediate = e,\
	p##_register_ecx_register_esi_immediate = e,\
	p##_register_ecx_register_edi_immediate = e,\
	p##_register_edx_register_eax_immediate = e,\
	p##_register_edx_register_ecx_immediate = e,\
	p##_register_edx_register_edx_immediate = e,\
	p##_register_edx_register_ebx_immediate = e,\
	p##_register_edx_register_esp_immediate = e,\
	p##_register_edx_register_ebp_immediate = e,\
	p##_register_edx_register_esi_immediate = e,\
	p##_register_edx_register_edi_immediate = e,\
	p##_register_ebx_register_eax_immediate = e,\
	p##_register_ebx_register_ecx_immediate = e,\
	p##_register_ebx_register_edx_immediate = e,\
	p##_register_ebx_register_ebx_immediate = e,\
	p##_register_ebx_register_esp_immediate = e,\
	p##_register_ebx_register_ebp_immediate = e,\
	p##_register_ebx_register_esi_immediate = e,\
	p##_register_ebx_register_edi_immediate = e,\
	p##_register_esp_register_eax_immediate = e,\
	p##_register_esp_register_ecx_immediate = e,\
	p##_register_esp_register_edx_immediate = e,\
	p##_register_esp_register_ebx_immediate = e,\
	p##_register_esp_register_esp_immediate = e,\
	p##_register_esp_register_ebp_immediate = e,\
	p##_register_esp_register_esi_immediate = e,\
	p##_register_esp_register_edi_immediate = e,\
	p##_register_ebp_register_eax_immediate = e,\
	p##_register_ebp_register_ecx_immediate = e,\
	p##_register_ebp_register_edx_immediate = e,\
	p##_register_ebp_register_ebx_immediate = e,\
	p##_register_ebp_register_esp_immediate = e,\
	p##_register_ebp_register_ebp_immediate = e,\
	p##_register_ebp_register_esi_immediate = e,\
	p##_register_ebp_register_edi_immediate = e,\
	p##_register_esi_register_eax_immediate = e,\
	p##_register_esi_register_ecx_immediate = e,\
	p##_register_esi_register_edx_immediate = e,\
	p##_register_esi_register_ebx_immediate = e,\
	p##_register_esi_register_esp_immediate = e,\
	p##_register_esi_register_ebp_immediate = e,\
	p##_register_esi_register_esi_immediate = e,\
	p##_register_esi_register_edi_immediate = e,\
	p##_register_edi_register_eax_immediate = e,\
	p##_register_edi_register_ecx_immediate = e,\
	p##_register_edi_register_edx_immediate = e,\
	p##_register_edi_register_ebx_immediate = e,\
	p##_register_edi_register_esp_immediate = e,\
	p##_register_edi_register_ebp_immediate = e,\
	p##_register_edi_register_esi_immediate = e,\
	p##_register_edi_register_edi_immediate = e,\
	p##_offset_register_eax_immediate = e,\
	p##_offset_register_ecx_immediate = e,\
	p##_offset_register_edx_immediate = e,\
	p##_offset_register_ebx_immediate = e,\
	p##_offset_register_esp_immediate = e,\
	p##_offset_register_ebp_immediate = e,\
	p##_offset_register_esi_immediate = e,\
	p##_offset_register_edi_immediate = e,\
	p##_scale_register_eax_immediate = e,\
	p##_scale_register_ecx_immediate = e,\
	p##_scale_register_edx_immediate = e,\
	p##_scale_register_ebx_immediate = e,\
	p##_scale_register_esp_immediate = e,\
	p##_scale_register_ebp_immediate = e,\
	p##_scale_register_esi_immediate = e,\
	p##_scale_register_edi_immediate = e,
#define NT63(p, e)\
	p##_register_fs = e,
#define NT64(p, e)\
	p##_register_al = e,\
	p##_register_cl = e,\
	p##_register_dl = e,\
	p##_register_bl = e,\
	p##_register_ah = e,\
	p##_register_ch = e,\
	p##_register_dh = e,\
	p##_register_bh = e,\
	p##_offset = e,\
	p##_scale = e,
#define NT65(p, e)\
	p##_offset_register_mm0 = e,\
	p##_offset_register_mm1 = e,\
	p##_offset_register_mm2 = e,\
	p##_offset_register_mm3 = e,\
	p##_offset_register_mm4 = e,\
	p##_offset_register_mm5 = e,\
	p##_offset_register_mm6 = e,\
	p##_offset_register_mm7 = e,\
	p##_scale_register_mm0 = e,\
	p##_scale_register_mm1 = e,\
	p##_scale_register_mm2 = e,\
	p##_scale_register_mm3 = e,\
	p##_scale_register_mm4 = e,\
	p##_scale_register_mm5 = e,\
	p##_scale_register_mm6 = e,\
	p##_scale_register_mm7 = e,
#define NT66(p, e)\
	p##_register_eax_register_mm0 = e,\
	p##_register_eax_register_mm1 = e,\
	p##_register_eax_register_mm2 = e,\
	p##_register_eax_register_mm3 = e,\
	p##_register_eax_register_mm4 = e,\
	p##_register_eax_register_mm5 = e,\
	p##_register_eax_register_mm6 = e,\
	p##_register_eax_register_mm7 = e,\
	p##_register_ecx_register_mm0 = e,\
	p##_register_ecx_register_mm1 = e,\
	p##_register_ecx_register_mm2 = e,\
	p##_register_ecx_register_mm3 = e,\
	p##_register_ecx_register_mm4 = e,\
	p##_register_ecx_register_mm5 = e,\
	p##_register_ecx_register_mm6 = e,\
	p##_register_ecx_register_mm7 = e,\
	p##_register_edx_register_mm0 = e,\
	p##_register_edx_register_mm1 = e,\
	p##_register_edx_register_mm2 = e,\
	p##_register_edx_register_mm3 = e,\
	p##_register_edx_register_mm4 = e,\
	p##_register_edx_register_mm5 = e,\
	p##_register_edx_register_mm6 = e,\
	p##_register_edx_register_mm7 = e,\
	p##_register_ebx_register_mm0 = e,\
	p##_register_ebx_register_mm1 = e,\
	p##_register_ebx_register_mm2 = e,\
	p##_register_ebx_register_mm3 = e,\
	p##_register_ebx_register_mm4 = e,\
	p##_register_ebx_register_mm5 = e,\
	p##_register_ebx_register_mm6 = e,\
	p##_register_ebx_register_mm7 = e,\
	p##_register_esp_register_mm0 = e,\
	p##_register_esp_register_mm1 = e,\
	p##_register_esp_register_mm2 = e,\
	p##_register_esp_register_mm3 = e,\
	p##_register_esp_register_mm4 = e,\
	p##_register_esp_register_mm5 = e,\
	p##_register_esp_register_mm6 = e,\
	p##_register_esp_register_mm7 = e,\
	p##_register_ebp_register_mm0 = e,\
	p##_register_ebp_register_mm1 = e,\
	p##_register_ebp_register_mm2 = e,\
	p##_register_ebp_register_mm3 = e,\
	p##_register_ebp_register_mm4 = e,\
	p##_register_ebp_register_mm5 = e,\
	p##_register_ebp_register_mm6 = e,\
	p##_register_ebp_register_mm7 = e,\
	p##_register_esi_register_mm0 = e,\
	p##_register_esi_register_mm1 = e,\
	p##_register_esi_register_mm2 = e,\
	p##_register_esi_register_mm3 = e,\
	p##_register_esi_register_mm4 = e,\
	p##_register_esi_register_mm5 = e,\
	p##_register_esi_register_mm6 = e,\
	p##_register_esi_register_mm7 = e,\
	p##_register_edi_register_mm0 = e,\
	p##_register_edi_register_mm1 = e,\
	p##_register_edi_register_mm2 = e,\
	p##_register_edi_register_mm3 = e,\
	p##_register_edi_register_mm4 = e,\
	p##_register_edi_register_mm5 = e,\
	p##_register_edi_register_mm6 = e,\
	p##_register_edi_register_mm7 = e,\
	p##_offset_register_mm0 = e,\
	p##_offset_register_mm1 = e,\
	p##_offset_register_mm2 = e,\
	p##_offset_register_mm3 = e,\
	p##_offset_register_mm4 = e,\
	p##_offset_register_mm5 = e,\
	p##_offset_register_mm6 = e,\
	p##_offset_register_mm7 = e,\
	p##_scale_register_mm0 = e,\
	p##_scale_register_mm1 = e,\
	p##_scale_register_mm2 = e,\
	p##_scale_register_mm3 = e,\
	p##_scale_register_mm4 = e,\
	p##_scale_register_mm5 = e,\
	p##_scale_register_mm6 = e,\
	p##_scale_register_mm7 = e,
#define NT67(p, e)\
	p##_register_mm0_register_mm0_immediate = e,\
	p##_register_mm0_register_mm1_immediate = e,\
	p##_register_mm0_register_mm2_immediate = e,\
	p##_register_mm0_register_mm3_immediate = e,\
	p##_register_mm0_register_mm4_immediate = e,\
	p##_register_mm0_register_mm5_immediate = e,\
	p##_register_mm0_register_mm6_immediate = e,\
	p##_register_mm0_register_mm7_immediate = e,\
	p##_register_mm0_offset_immediate = e,\
	p##_register_mm0_scale_immediate = e,\
	p##_register_mm1_register_mm0_immediate = e,\
	p##_register_mm1_register_mm1_immediate = e,\
	p##_register_mm1_register_mm2_immediate = e,\
	p##_register_mm1_register_mm3_immediate = e,\
	p##_register_mm1_register_mm4_immediate = e,\
	p##_register_mm1_register_mm5_immediate = e,\
	p##_register_mm1_register_mm6_immediate = e,\
	p##_register_mm1_register_mm7_immediate = e,\
	p##_register_mm1_offset_immediate = e,\
	p##_register_mm1_scale_immediate = e,\
	p##_register_mm2_register_mm0_immediate = e,\
	p##_register_mm2_register_mm1_immediate = e,\
	p##_register_mm2_register_mm2_immediate = e,\
	p##_register_mm2_register_mm3_immediate = e,\
	p##_register_mm2_register_mm4_immediate = e,\
	p##_register_mm2_register_mm5_immediate = e,\
	p##_register_mm2_register_mm6_immediate = e,\
	p##_register_mm2_register_mm7_immediate = e,\
	p##_register_mm2_offset_immediate = e,\
	p##_register_mm2_scale_immediate = e,\
	p##_register_mm3_register_mm0_immediate = e,\
	p##_register_mm3_register_mm1_immediate = e,\
	p##_register_mm3_register_mm2_immediate = e,\
	p##_register_mm3_register_mm3_immediate = e,\
	p##_register_mm3_register_mm4_immediate = e,\
	p##_register_mm3_register_mm5_immediate = e,\
	p##_register_mm3_register_mm6_immediate = e,\
	p##_register_mm3_register_mm7_immediate = e,\
	p##_register_mm3_offset_immediate = e,\
	p##_register_mm3_scale_immediate = e,\
	p##_register_mm4_register_mm0_immediate = e,\
	p##_register_mm4_register_mm1_immediate = e,\
	p##_register_mm4_register_mm2_immediate = e,\
	p##_register_mm4_register_mm3_immediate = e,\
	p##_register_mm4_register_mm4_immediate = e,\
	p##_register_mm4_register_mm5_immediate = e,\
	p##_register_mm4_register_mm6_immediate = e,\
	p##_register_mm4_register_mm7_immediate = e,\
	p##_register_mm4_offset_immediate = e,\
	p##_register_mm4_scale_immediate = e,\
	p##_register_mm5_register_mm0_immediate = e,\
	p##_register_mm5_register_mm1_immediate = e,\
	p##_register_mm5_register_mm2_immediate = e,\
	p##_register_mm5_register_mm3_immediate = e,\
	p##_register_mm5_register_mm4_immediate = e,\
	p##_register_mm5_register_mm5_immediate = e,\
	p##_register_mm5_register_mm6_immediate = e,\
	p##_register_mm5_register_mm7_immediate = e,\
	p##_register_mm5_offset_immediate = e,\
	p##_register_mm5_scale_immediate = e,\
	p##_register_mm6_register_mm0_immediate = e,\
	p##_register_mm6_register_mm1_immediate = e,\
	p##_register_mm6_register_mm2_immediate = e,\
	p##_register_mm6_register_mm3_immediate = e,\
	p##_register_mm6_register_mm4_immediate = e,\
	p##_register_mm6_register_mm5_immediate = e,\
	p##_register_mm6_register_mm6_immediate = e,\
	p##_register_mm6_register_mm7_immediate = e,\
	p##_register_mm6_offset_immediate = e,\
	p##_register_mm6_scale_immediate = e,\
	p##_register_mm7_register_mm0_immediate = e,\
	p##_register_mm7_register_mm1_immediate = e,\
	p##_register_mm7_register_mm2_immediate = e,\
	p##_register_mm7_register_mm3_immediate = e,\
	p##_register_mm7_register_mm4_immediate = e,\
	p##_register_mm7_register_mm5_immediate = e,\
	p##_register_mm7_register_mm6_immediate = e,\
	p##_register_mm7_register_mm7_immediate = e,\
	p##_register_mm7_offset_immediate = e,\
	p##_register_mm7_scale_immediate = e,
#define NT68(p, e)\
	p##_register_mm0_register_eax = e,\
	p##_register_mm0_register_ecx = e,\
	p##_register_mm0_register_edx = e,\
	p##_register_mm0_register_ebx = e,\
	p##_register_mm0_register_esp = e,\
	p##_register_mm0_register_ebp = e,\
	p##_register_mm0_register_esi = e,\
	p##_register_mm0_register_edi = e,\
	p##_register_mm0_offset = e,\
	p##_register_mm0_scale = e,\
	p##_register_mm1_register_eax = e,\
	p##_register_mm1_register_ecx = e,\
	p##_register_mm1_register_edx = e,\
	p##_register_mm1_register_ebx = e,\
	p##_register_mm1_register_esp = e,\
	p##_register_mm1_register_ebp = e,\
	p##_register_mm1_register_esi = e,\
	p##_register_mm1_register_edi = e,\
	p##_register_mm1_offset = e,\
	p##_register_mm1_scale = e,\
	p##_register_mm2_register_eax = e,\
	p##_register_mm2_register_ecx = e,\
	p##_register_mm2_register_edx = e,\
	p##_register_mm2_register_ebx = e,\
	p##_register_mm2_register_esp = e,\
	p##_register_mm2_register_ebp = e,\
	p##_register_mm2_register_esi = e,\
	p##_register_mm2_register_edi = e,\
	p##_register_mm2_offset = e,\
	p##_register_mm2_scale = e,\
	p##_register_mm3_register_eax = e,\
	p##_register_mm3_register_ecx = e,\
	p##_register_mm3_register_edx = e,\
	p##_register_mm3_register_ebx = e,\
	p##_register_mm3_register_esp = e,\
	p##_register_mm3_register_ebp = e,\
	p##_register_mm3_register_esi = e,\
	p##_register_mm3_register_edi = e,\
	p##_register_mm3_offset = e,\
	p##_register_mm3_scale = e,\
	p##_register_mm4_register_eax = e,\
	p##_register_mm4_register_ecx = e,\
	p##_register_mm4_register_edx = e,\
	p##_register_mm4_register_ebx = e,\
	p##_register_mm4_register_esp = e,\
	p##_register_mm4_register_ebp = e,\
	p##_register_mm4_register_esi = e,\
	p##_register_mm4_register_edi = e,\
	p##_register_mm4_offset = e,\
	p##_register_mm4_scale = e,\
	p##_register_mm5_register_eax = e,\
	p##_register_mm5_register_ecx = e,\
	p##_register_mm5_register_edx = e,\
	p##_register_mm5_register_ebx = e,\
	p##_register_mm5_register_esp = e,\
	p##_register_mm5_register_ebp = e,\
	p##_register_mm5_register_esi = e,\
	p##_register_mm5_register_edi = e,\
	p##_register_mm5_offset = e,\
	p##_register_mm5_scale = e,\
	p##_register_mm6_register_eax = e,\
	p##_register_mm6_register_ecx = e,\
	p##_register_mm6_register_edx = e,\
	p##_register_mm6_register_ebx = e,\
	p##_register_mm6_register_esp = e,\
	p##_register_mm6_register_ebp = e,\
	p##_register_mm6_register_esi = e,\
	p##_register_mm6_register_edi = e,\
	p##_register_mm6_offset = e,\
	p##_register_mm6_scale = e,\
	p##_register_mm7_register_eax = e,\
	p##_register_mm7_register_ecx = e,\
	p##_register_mm7_register_edx = e,\
	p##_register_mm7_register_ebx = e,\
	p##_register_mm7_register_esp = e,\
	p##_register_mm7_register_ebp = e,\
	p##_register_mm7_register_esi = e,\
	p##_register_mm7_register_edi = e,\
	p##_register_mm7_offset = e,\
	p##_register_mm7_scale = e,
#define NT69(p, e)\
	p##_register_mm0_register_mm0 = e,\
	p##_register_mm0_register_mm1 = e,\
	p##_register_mm0_register_mm2 = e,\
	p##_register_mm0_register_mm3 = e,\
	p##_register_mm0_register_mm4 = e,\
	p##_register_mm0_register_mm5 = e,\
	p##_register_mm0_register_mm6 = e,\
	p##_register_mm0_register_mm7 = e,\
	p##_register_mm0_offset = e,\
	p##_register_mm0_scale = e,\
	p##_register_mm1_register_mm0 = e,\
	p##_register_mm1_register_mm1 = e,\
	p##_register_mm1_register_mm2 = e,\
	p##_register_mm1_register_mm3 = e,\
	p##_register_mm1_register_mm4 = e,\
	p##_register_mm1_register_mm5 = e,\
	p##_register_mm1_register_mm6 = e,\
	p##_register_mm1_register_mm7 = e,\
	p##_register_mm1_offset = e,\
	p##_register_mm1_scale = e,\
	p##_register_mm2_register_mm0 = e,\
	p##_register_mm2_register_mm1 = e,\
	p##_register_mm2_register_mm2 = e,\
	p##_register_mm2_register_mm3 = e,\
	p##_register_mm2_register_mm4 = e,\
	p##_register_mm2_register_mm5 = e,\
	p##_register_mm2_register_mm6 = e,\
	p##_register_mm2_register_mm7 = e,\
	p##_register_mm2_offset = e,\
	p##_register_mm2_scale = e,\
	p##_register_mm3_register_mm0 = e,\
	p##_register_mm3_register_mm1 = e,\
	p##_register_mm3_register_mm2 = e,\
	p##_register_mm3_register_mm3 = e,\
	p##_register_mm3_register_mm4 = e,\
	p##_register_mm3_register_mm5 = e,\
	p##_register_mm3_register_mm6 = e,\
	p##_register_mm3_register_mm7 = e,\
	p##_register_mm3_offset = e,\
	p##_register_mm3_scale = e,\
	p##_register_mm4_register_mm0 = e,\
	p##_register_mm4_register_mm1 = e,\
	p##_register_mm4_register_mm2 = e,\
	p##_register_mm4_register_mm3 = e,\
	p##_register_mm4_register_mm4 = e,\
	p##_register_mm4_register_mm5 = e,\
	p##_register_mm4_register_mm6 = e,\
	p##_register_mm4_register_mm7 = e,\
	p##_register_mm4_offset = e,\
	p##_register_mm4_scale = e,\
	p##_register_mm5_register_mm0 = e,\
	p##_register_mm5_register_mm1 = e,\
	p##_register_mm5_register_mm2 = e,\
	p##_register_mm5_register_mm3 = e,\
	p##_register_mm5_register_mm4 = e,\
	p##_register_mm5_register_mm5 = e,\
	p##_register_mm5_register_mm6 = e,\
	p##_register_mm5_register_mm7 = e,\
	p##_register_mm5_offset = e,\
	p##_register_mm5_scale = e,\
	p##_register_mm6_register_mm0 = e,\
	p##_register_mm6_register_mm1 = e,\
	p##_register_mm6_register_mm2 = e,\
	p##_register_mm6_register_mm3 = e,\
	p##_register_mm6_register_mm4 = e,\
	p##_register_mm6_register_mm5 = e,\
	p##_register_mm6_register_mm6 = e,\
	p##_register_mm6_register_mm7 = e,\
	p##_register_mm6_offset = e,\
	p##_register_mm6_scale = e,\
	p##_register_mm7_register_mm0 = e,\
	p##_register_mm7_register_mm1 = e,\
	p##_register_mm7_register_mm2 = e,\
	p##_register_mm7_register_mm3 = e,\
	p##_register_mm7_register_mm4 = e,\
	p##_register_mm7_register_mm5 = e,\
	p##_register_mm7_register_mm6 = e,\
	p##_register_mm7_register_mm7 = e,\
	p##_register_mm7_offset = e,\
	p##_register_mm7_scale = e,
#define NT70(p, e)\
	p##_register_eax_register_xmm0 = e,\
	p##_register_eax_register_xmm1 = e,\
	p##_register_eax_register_xmm2 = e,\
	p##_register_eax_register_xmm3 = e,\
	p##_register_eax_register_xmm4 = e,\
	p##_register_eax_register_xmm5 = e,\
	p##_register_eax_register_xmm6 = e,\
	p##_register_eax_register_xmm7 = e,\
	p##_register_ecx_register_xmm0 = e,\
	p##_register_ecx_register_xmm1 = e,\
	p##_register_ecx_register_xmm2 = e,\
	p##_register_ecx_register_xmm3 = e,\
	p##_register_ecx_register_xmm4 = e,\
	p##_register_ecx_register_xmm5 = e,\
	p##_register_ecx_register_xmm6 = e,\
	p##_register_ecx_register_xmm7 = e,\
	p##_register_edx_register_xmm0 = e,\
	p##_register_edx_register_xmm1 = e,\
	p##_register_edx_register_xmm2 = e,\
	p##_register_edx_register_xmm3 = e,\
	p##_register_edx_register_xmm4 = e,\
	p##_register_edx_register_xmm5 = e,\
	p##_register_edx_register_xmm6 = e,\
	p##_register_edx_register_xmm7 = e,\
	p##_register_ebx_register_xmm0 = e,\
	p##_register_ebx_register_xmm1 = e,\
	p##_register_ebx_register_xmm2 = e,\
	p##_register_ebx_register_xmm3 = e,\
	p##_register_ebx_register_xmm4 = e,\
	p##_register_ebx_register_xmm5 = e,\
	p##_register_ebx_register_xmm6 = e,\
	p##_register_ebx_register_xmm7 = e,\
	p##_register_esp_register_xmm0 = e,\
	p##_register_esp_register_xmm1 = e,\
	p##_register_esp_register_xmm2 = e,\
	p##_register_esp_register_xmm3 = e,\
	p##_register_esp_register_xmm4 = e,\
	p##_register_esp_register_xmm5 = e,\
	p##_register_esp_register_xmm6 = e,\
	p##_register_esp_register_xmm7 = e,\
	p##_register_ebp_register_xmm0 = e,\
	p##_register_ebp_register_xmm1 = e,\
	p##_register_ebp_register_xmm2 = e,\
	p##_register_ebp_register_xmm3 = e,\
	p##_register_ebp_register_xmm4 = e,\
	p##_register_ebp_register_xmm5 = e,\
	p##_register_ebp_register_xmm6 = e,\
	p##_register_ebp_register_xmm7 = e,\
	p##_register_esi_register_xmm0 = e,\
	p##_register_esi_register_xmm1 = e,\
	p##_register_esi_register_xmm2 = e,\
	p##_register_esi_register_xmm3 = e,\
	p##_register_esi_register_xmm4 = e,\
	p##_register_esi_register_xmm5 = e,\
	p##_register_esi_register_xmm6 = e,\
	p##_register_esi_register_xmm7 = e,\
	p##_register_edi_register_xmm0 = e,\
	p##_register_edi_register_xmm1 = e,\
	p##_register_edi_register_xmm2 = e,\
	p##_register_edi_register_xmm3 = e,\
	p##_register_edi_register_xmm4 = e,\
	p##_register_edi_register_xmm5 = e,\
	p##_register_edi_register_xmm6 = e,\
	p##_register_edi_register_xmm7 = e,\
	p##_offset_register_xmm0 = e,\
	p##_offset_register_xmm1 = e,\
	p##_offset_register_xmm2 = e,\
	p##_offset_register_xmm3 = e,\
	p##_offset_register_xmm4 = e,\
	p##_offset_register_xmm5 = e,\
	p##_offset_register_xmm6 = e,\
	p##_offset_register_xmm7 = e,\
	p##_scale_register_xmm0 = e,\
	p##_scale_register_xmm1 = e,\
	p##_scale_register_xmm2 = e,\
	p##_scale_register_xmm3 = e,\
	p##_scale_register_xmm4 = e,\
	p##_scale_register_xmm5 = e,\
	p##_scale_register_xmm6 = e,\
	p##_scale_register_xmm7 = e,
#define NT71(p, e)\
	p##_register_eax_register_eax = e,\
	p##_register_eax_register_ecx = e,\
	p##_register_eax_register_edx = e,\
	p##_register_eax_register_ebx = e,\
	p##_register_eax_register_esp = e,\
	p##_register_eax_register_ebp = e,\
	p##_register_eax_register_esi = e,\
	p##_register_eax_register_edi = e,\
	p##_register_eax_offset = e,\
	p##_register_eax_scale = e,\
	p##_register_ecx_register_eax = e,\
	p##_register_ecx_register_ecx = e,\
	p##_register_ecx_register_edx = e,\
	p##_register_ecx_register_ebx = e,\
	p##_register_ecx_register_esp = e,\
	p##_register_ecx_register_ebp = e,\
	p##_register_ecx_register_esi = e,\
	p##_register_ecx_register_edi = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_register_eax = e,\
	p##_register_edx_register_ecx = e,\
	p##_register_edx_register_edx = e,\
	p##_register_edx_register_ebx = e,\
	p##_register_edx_register_esp = e,\
	p##_register_edx_register_ebp = e,\
	p##_register_edx_register_esi = e,\
	p##_register_edx_register_edi = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_register_eax = e,\
	p##_register_ebx_register_ecx = e,\
	p##_register_ebx_register_edx = e,\
	p##_register_ebx_register_ebx = e,\
	p##_register_ebx_register_esp = e,\
	p##_register_ebx_register_ebp = e,\
	p##_register_ebx_register_esi = e,\
	p##_register_ebx_register_edi = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_register_eax = e,\
	p##_register_esp_register_ecx = e,\
	p##_register_esp_register_edx = e,\
	p##_register_esp_register_ebx = e,\
	p##_register_esp_register_esp = e,\
	p##_register_esp_register_ebp = e,\
	p##_register_esp_register_esi = e,\
	p##_register_esp_register_edi = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_register_eax = e,\
	p##_register_ebp_register_ecx = e,\
	p##_register_ebp_register_edx = e,\
	p##_register_ebp_register_ebx = e,\
	p##_register_ebp_register_esp = e,\
	p##_register_ebp_register_ebp = e,\
	p##_register_ebp_register_esi = e,\
	p##_register_ebp_register_edi = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_register_eax = e,\
	p##_register_esi_register_ecx = e,\
	p##_register_esi_register_edx = e,\
	p##_register_esi_register_ebx = e,\
	p##_register_esi_register_esp = e,\
	p##_register_esi_register_ebp = e,\
	p##_register_esi_register_esi = e,\
	p##_register_esi_register_edi = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_register_eax = e,\
	p##_register_edi_register_ecx = e,\
	p##_register_edi_register_edx = e,\
	p##_register_edi_register_ebx = e,\
	p##_register_edi_register_esp = e,\
	p##_register_edi_register_ebp = e,\
	p##_register_edi_register_esi = e,\
	p##_register_edi_register_edi = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT72(p, e)\
	p##_register_mm0_register_xmm0 = e,\
	p##_register_mm0_register_xmm1 = e,\
	p##_register_mm0_register_xmm2 = e,\
	p##_register_mm0_register_xmm3 = e,\
	p##_register_mm0_register_xmm4 = e,\
	p##_register_mm0_register_xmm5 = e,\
	p##_register_mm0_register_xmm6 = e,\
	p##_register_mm0_register_xmm7 = e,\
	p##_register_mm0_offset = e,\
	p##_register_mm0_scale = e,\
	p##_register_mm1_register_xmm0 = e,\
	p##_register_mm1_register_xmm1 = e,\
	p##_register_mm1_register_xmm2 = e,\
	p##_register_mm1_register_xmm3 = e,\
	p##_register_mm1_register_xmm4 = e,\
	p##_register_mm1_register_xmm5 = e,\
	p##_register_mm1_register_xmm6 = e,\
	p##_register_mm1_register_xmm7 = e,\
	p##_register_mm1_offset = e,\
	p##_register_mm1_scale = e,\
	p##_register_mm2_register_xmm0 = e,\
	p##_register_mm2_register_xmm1 = e,\
	p##_register_mm2_register_xmm2 = e,\
	p##_register_mm2_register_xmm3 = e,\
	p##_register_mm2_register_xmm4 = e,\
	p##_register_mm2_register_xmm5 = e,\
	p##_register_mm2_register_xmm6 = e,\
	p##_register_mm2_register_xmm7 = e,\
	p##_register_mm2_offset = e,\
	p##_register_mm2_scale = e,\
	p##_register_mm3_register_xmm0 = e,\
	p##_register_mm3_register_xmm1 = e,\
	p##_register_mm3_register_xmm2 = e,\
	p##_register_mm3_register_xmm3 = e,\
	p##_register_mm3_register_xmm4 = e,\
	p##_register_mm3_register_xmm5 = e,\
	p##_register_mm3_register_xmm6 = e,\
	p##_register_mm3_register_xmm7 = e,\
	p##_register_mm3_offset = e,\
	p##_register_mm3_scale = e,\
	p##_register_mm4_register_xmm0 = e,\
	p##_register_mm4_register_xmm1 = e,\
	p##_register_mm4_register_xmm2 = e,\
	p##_register_mm4_register_xmm3 = e,\
	p##_register_mm4_register_xmm4 = e,\
	p##_register_mm4_register_xmm5 = e,\
	p##_register_mm4_register_xmm6 = e,\
	p##_register_mm4_register_xmm7 = e,\
	p##_register_mm4_offset = e,\
	p##_register_mm4_scale = e,\
	p##_register_mm5_register_xmm0 = e,\
	p##_register_mm5_register_xmm1 = e,\
	p##_register_mm5_register_xmm2 = e,\
	p##_register_mm5_register_xmm3 = e,\
	p##_register_mm5_register_xmm4 = e,\
	p##_register_mm5_register_xmm5 = e,\
	p##_register_mm5_register_xmm6 = e,\
	p##_register_mm5_register_xmm7 = e,\
	p##_register_mm5_offset = e,\
	p##_register_mm5_scale = e,\
	p##_register_mm6_register_xmm0 = e,\
	p##_register_mm6_register_xmm1 = e,\
	p##_register_mm6_register_xmm2 = e,\
	p##_register_mm6_register_xmm3 = e,\
	p##_register_mm6_register_xmm4 = e,\
	p##_register_mm6_register_xmm5 = e,\
	p##_register_mm6_register_xmm6 = e,\
	p##_register_mm6_register_xmm7 = e,\
	p##_register_mm6_offset = e,\
	p##_register_mm6_scale = e,\
	p##_register_mm7_register_xmm0 = e,\
	p##_register_mm7_register_xmm1 = e,\
	p##_register_mm7_register_xmm2 = e,\
	p##_register_mm7_register_xmm3 = e,\
	p##_register_mm7_register_xmm4 = e,\
	p##_register_mm7_register_xmm5 = e,\
	p##_register_mm7_register_xmm6 = e,\
	p##_register_mm7_register_xmm7 = e,\
	p##_register_mm7_offset = e,\
	p##_register_mm7_scale = e,\
	p##_offset_register_xmm0 = e,\
	p##_offset_register_xmm1 = e,\
	p##_offset_register_xmm2 = e,\
	p##_offset_register_xmm3 = e,\
	p##_offset_register_xmm4 = e,\
	p##_offset_register_xmm5 = e,\
	p##_offset_register_xmm6 = e,\
	p##_offset_register_xmm7 = e,\
	p##_offset_offset = e,\
	p##_offset_scale = e,\
	p##_scale_register_xmm0 = e,\
	p##_scale_register_xmm1 = e,\
	p##_scale_register_xmm2 = e,\
	p##_scale_register_xmm3 = e,\
	p##_scale_register_xmm4 = e,\
	p##_scale_register_xmm5 = e,\
	p##_scale_register_xmm6 = e,\
	p##_scale_register_xmm7 = e,\
	p##_scale_offset = e,\
	p##_scale_scale = e,
#define NT73(p, e)\
	p##_register_xmm0_register_mm0 = e,\
	p##_register_xmm0_register_mm1 = e,\
	p##_register_xmm0_register_mm2 = e,\
	p##_register_xmm0_register_mm3 = e,\
	p##_register_xmm0_register_mm4 = e,\
	p##_register_xmm0_register_mm5 = e,\
	p##_register_xmm0_register_mm6 = e,\
	p##_register_xmm0_register_mm7 = e,\
	p##_register_xmm0_offset = e,\
	p##_register_xmm0_scale = e,\
	p##_register_xmm1_register_mm0 = e,\
	p##_register_xmm1_register_mm1 = e,\
	p##_register_xmm1_register_mm2 = e,\
	p##_register_xmm1_register_mm3 = e,\
	p##_register_xmm1_register_mm4 = e,\
	p##_register_xmm1_register_mm5 = e,\
	p##_register_xmm1_register_mm6 = e,\
	p##_register_xmm1_register_mm7 = e,\
	p##_register_xmm1_offset = e,\
	p##_register_xmm1_scale = e,\
	p##_register_xmm2_register_mm0 = e,\
	p##_register_xmm2_register_mm1 = e,\
	p##_register_xmm2_register_mm2 = e,\
	p##_register_xmm2_register_mm3 = e,\
	p##_register_xmm2_register_mm4 = e,\
	p##_register_xmm2_register_mm5 = e,\
	p##_register_xmm2_register_mm6 = e,\
	p##_register_xmm2_register_mm7 = e,\
	p##_register_xmm2_offset = e,\
	p##_register_xmm2_scale = e,\
	p##_register_xmm3_register_mm0 = e,\
	p##_register_xmm3_register_mm1 = e,\
	p##_register_xmm3_register_mm2 = e,\
	p##_register_xmm3_register_mm3 = e,\
	p##_register_xmm3_register_mm4 = e,\
	p##_register_xmm3_register_mm5 = e,\
	p##_register_xmm3_register_mm6 = e,\
	p##_register_xmm3_register_mm7 = e,\
	p##_register_xmm3_offset = e,\
	p##_register_xmm3_scale = e,\
	p##_register_xmm4_register_mm0 = e,\
	p##_register_xmm4_register_mm1 = e,\
	p##_register_xmm4_register_mm2 = e,\
	p##_register_xmm4_register_mm3 = e,\
	p##_register_xmm4_register_mm4 = e,\
	p##_register_xmm4_register_mm5 = e,\
	p##_register_xmm4_register_mm6 = e,\
	p##_register_xmm4_register_mm7 = e,\
	p##_register_xmm4_offset = e,\
	p##_register_xmm4_scale = e,\
	p##_register_xmm5_register_mm0 = e,\
	p##_register_xmm5_register_mm1 = e,\
	p##_register_xmm5_register_mm2 = e,\
	p##_register_xmm5_register_mm3 = e,\
	p##_register_xmm5_register_mm4 = e,\
	p##_register_xmm5_register_mm5 = e,\
	p##_register_xmm5_register_mm6 = e,\
	p##_register_xmm5_register_mm7 = e,\
	p##_register_xmm5_offset = e,\
	p##_register_xmm5_scale = e,\
	p##_register_xmm6_register_mm0 = e,\
	p##_register_xmm6_register_mm1 = e,\
	p##_register_xmm6_register_mm2 = e,\
	p##_register_xmm6_register_mm3 = e,\
	p##_register_xmm6_register_mm4 = e,\
	p##_register_xmm6_register_mm5 = e,\
	p##_register_xmm6_register_mm6 = e,\
	p##_register_xmm6_register_mm7 = e,\
	p##_register_xmm6_offset = e,\
	p##_register_xmm6_scale = e,\
	p##_register_xmm7_register_mm0 = e,\
	p##_register_xmm7_register_mm1 = e,\
	p##_register_xmm7_register_mm2 = e,\
	p##_register_xmm7_register_mm3 = e,\
	p##_register_xmm7_register_mm4 = e,\
	p##_register_xmm7_register_mm5 = e,\
	p##_register_xmm7_register_mm6 = e,\
	p##_register_xmm7_register_mm7 = e,\
	p##_register_xmm7_offset = e,\
	p##_register_xmm7_scale = e,
#define NT74(p, e)\
	p##_register_dr0_register_eax = e,\
	p##_register_dr0_register_ecx = e,\
	p##_register_dr0_register_edx = e,\
	p##_register_dr0_register_ebx = e,\
	p##_register_dr0_register_esp = e,\
	p##_register_dr0_register_ebp = e,\
	p##_register_dr0_register_esi = e,\
	p##_register_dr0_register_edi = e,\
	p##_register_dr1_register_eax = e,\
	p##_register_dr1_register_ecx = e,\
	p##_register_dr1_register_edx = e,\
	p##_register_dr1_register_ebx = e,\
	p##_register_dr1_register_esp = e,\
	p##_register_dr1_register_ebp = e,\
	p##_register_dr1_register_esi = e,\
	p##_register_dr1_register_edi = e,\
	p##_register_dr2_register_eax = e,\
	p##_register_dr2_register_ecx = e,\
	p##_register_dr2_register_edx = e,\
	p##_register_dr2_register_ebx = e,\
	p##_register_dr2_register_esp = e,\
	p##_register_dr2_register_ebp = e,\
	p##_register_dr2_register_esi = e,\
	p##_register_dr2_register_edi = e,\
	p##_register_dr3_register_eax = e,\
	p##_register_dr3_register_ecx = e,\
	p##_register_dr3_register_edx = e,\
	p##_register_dr3_register_ebx = e,\
	p##_register_dr3_register_esp = e,\
	p##_register_dr3_register_ebp = e,\
	p##_register_dr3_register_esi = e,\
	p##_register_dr3_register_edi = e,\
	p##_register_dr6_register_eax = e,\
	p##_register_dr6_register_ecx = e,\
	p##_register_dr6_register_edx = e,\
	p##_register_dr6_register_ebx = e,\
	p##_register_dr6_register_esp = e,\
	p##_register_dr6_register_ebp = e,\
	p##_register_dr6_register_esi = e,\
	p##_register_dr6_register_edi = e,\
	p##_register_dr7_register_eax = e,\
	p##_register_dr7_register_ecx = e,\
	p##_register_dr7_register_edx = e,\
	p##_register_dr7_register_ebx = e,\
	p##_register_dr7_register_esp = e,\
	p##_register_dr7_register_ebp = e,\
	p##_register_dr7_register_esi = e,\
	p##_register_dr7_register_edi = e,
#define NT75(p, e)\
	p##_register_cr0_register_eax = e,\
	p##_register_cr0_register_ecx = e,\
	p##_register_cr0_register_edx = e,\
	p##_register_cr0_register_ebx = e,\
	p##_register_cr0_register_esp = e,\
	p##_register_cr0_register_ebp = e,\
	p##_register_cr0_register_esi = e,\
	p##_register_cr0_register_edi = e,\
	p##_register_cr2_register_eax = e,\
	p##_register_cr2_register_ecx = e,\
	p##_register_cr2_register_edx = e,\
	p##_register_cr2_register_ebx = e,\
	p##_register_cr2_register_esp = e,\
	p##_register_cr2_register_ebp = e,\
	p##_register_cr2_register_esi = e,\
	p##_register_cr2_register_edi = e,\
	p##_register_cr3_register_eax = e,\
	p##_register_cr3_register_ecx = e,\
	p##_register_cr3_register_edx = e,\
	p##_register_cr3_register_ebx = e,\
	p##_register_cr3_register_esp = e,\
	p##_register_cr3_register_ebp = e,\
	p##_register_cr3_register_esi = e,\
	p##_register_cr3_register_edi = e,\
	p##_register_cr4_register_eax = e,\
	p##_register_cr4_register_ecx = e,\
	p##_register_cr4_register_edx = e,\
	p##_register_cr4_register_ebx = e,\
	p##_register_cr4_register_esp = e,\
	p##_register_cr4_register_ebp = e,\
	p##_register_cr4_register_esi = e,\
	p##_register_cr4_register_edi = e,
#define NT76(p, e)\
	p##_register_eax_register_dr0 = e,\
	p##_register_eax_register_dr1 = e,\
	p##_register_eax_register_dr2 = e,\
	p##_register_eax_register_dr3 = e,\
	p##_register_eax_register_dr6 = e,\
	p##_register_eax_register_dr7 = e,\
	p##_register_ecx_register_dr0 = e,\
	p##_register_ecx_register_dr1 = e,\
	p##_register_ecx_register_dr2 = e,\
	p##_register_ecx_register_dr3 = e,\
	p##_register_ecx_register_dr6 = e,\
	p##_register_ecx_register_dr7 = e,\
	p##_register_edx_register_dr0 = e,\
	p##_register_edx_register_dr1 = e,\
	p##_register_edx_register_dr2 = e,\
	p##_register_edx_register_dr3 = e,\
	p##_register_edx_register_dr6 = e,\
	p##_register_edx_register_dr7 = e,\
	p##_register_ebx_register_dr0 = e,\
	p##_register_ebx_register_dr1 = e,\
	p##_register_ebx_register_dr2 = e,\
	p##_register_ebx_register_dr3 = e,\
	p##_register_ebx_register_dr6 = e,\
	p##_register_ebx_register_dr7 = e,\
	p##_register_esp_register_dr0 = e,\
	p##_register_esp_register_dr1 = e,\
	p##_register_esp_register_dr2 = e,\
	p##_register_esp_register_dr3 = e,\
	p##_register_esp_register_dr6 = e,\
	p##_register_esp_register_dr7 = e,\
	p##_register_ebp_register_dr0 = e,\
	p##_register_ebp_register_dr1 = e,\
	p##_register_ebp_register_dr2 = e,\
	p##_register_ebp_register_dr3 = e,\
	p##_register_ebp_register_dr6 = e,\
	p##_register_ebp_register_dr7 = e,\
	p##_register_esi_register_dr0 = e,\
	p##_register_esi_register_dr1 = e,\
	p##_register_esi_register_dr2 = e,\
	p##_register_esi_register_dr3 = e,\
	p##_register_esi_register_dr6 = e,\
	p##_register_esi_register_dr7 = e,\
	p##_register_edi_register_dr0 = e,\
	p##_register_edi_register_dr1 = e,\
	p##_register_edi_register_dr2 = e,\
	p##_register_edi_register_dr3 = e,\
	p##_register_edi_register_dr6 = e,\
	p##_register_edi_register_dr7 = e,
#define NT77(p, e)\
	p##_register_eax_register_cr0 = e,\
	p##_register_eax_register_cr2 = e,\
	p##_register_eax_register_cr3 = e,\
	p##_register_eax_register_cr4 = e,\
	p##_register_ecx_register_cr0 = e,\
	p##_register_ecx_register_cr2 = e,\
	p##_register_ecx_register_cr3 = e,\
	p##_register_ecx_register_cr4 = e,\
	p##_register_edx_register_cr0 = e,\
	p##_register_edx_register_cr2 = e,\
	p##_register_edx_register_cr3 = e,\
	p##_register_edx_register_cr4 = e,\
	p##_register_ebx_register_cr0 = e,\
	p##_register_ebx_register_cr2 = e,\
	p##_register_ebx_register_cr3 = e,\
	p##_register_ebx_register_cr4 = e,\
	p##_register_esp_register_cr0 = e,\
	p##_register_esp_register_cr2 = e,\
	p##_register_esp_register_cr3 = e,\
	p##_register_esp_register_cr4 = e,\
	p##_register_ebp_register_cr0 = e,\
	p##_register_ebp_register_cr2 = e,\
	p##_register_ebp_register_cr3 = e,\
	p##_register_ebp_register_cr4 = e,\
	p##_register_esi_register_cr0 = e,\
	p##_register_esi_register_cr2 = e,\
	p##_register_esi_register_cr3 = e,\
	p##_register_esi_register_cr4 = e,\
	p##_register_edi_register_cr0 = e,\
	p##_register_edi_register_cr2 = e,\
	p##_register_edi_register_cr3 = e,\
	p##_register_edi_register_cr4 = e,
#define NT78(p, e)\
	p##_register_xmm0_offset = e,\
	p##_register_xmm0_scale = e,\
	p##_register_xmm1_offset = e,\
	p##_register_xmm1_scale = e,\
	p##_register_xmm2_offset = e,\
	p##_register_xmm2_scale = e,\
	p##_register_xmm3_offset = e,\
	p##_register_xmm3_scale = e,\
	p##_register_xmm4_offset = e,\
	p##_register_xmm4_scale = e,\
	p##_register_xmm5_offset = e,\
	p##_register_xmm5_scale = e,\
	p##_register_xmm6_offset = e,\
	p##_register_xmm6_scale = e,\
	p##_register_xmm7_offset = e,\
	p##_register_xmm7_scale = e,
#define NT79(p, e)\
	p##_register_xmm0_register_xmm0 = e,\
	p##_register_xmm0_register_xmm1 = e,\
	p##_register_xmm0_register_xmm2 = e,\
	p##_register_xmm0_register_xmm3 = e,\
	p##_register_xmm0_register_xmm4 = e,\
	p##_register_xmm0_register_xmm5 = e,\
	p##_register_xmm0_register_xmm6 = e,\
	p##_register_xmm0_register_xmm7 = e,\
	p##_register_xmm1_register_xmm0 = e,\
	p##_register_xmm1_register_xmm1 = e,\
	p##_register_xmm1_register_xmm2 = e,\
	p##_register_xmm1_register_xmm3 = e,\
	p##_register_xmm1_register_xmm4 = e,\
	p##_register_xmm1_register_xmm5 = e,\
	p##_register_xmm1_register_xmm6 = e,\
	p##_register_xmm1_register_xmm7 = e,\
	p##_register_xmm2_register_xmm0 = e,\
	p##_register_xmm2_register_xmm1 = e,\
	p##_register_xmm2_register_xmm2 = e,\
	p##_register_xmm2_register_xmm3 = e,\
	p##_register_xmm2_register_xmm4 = e,\
	p##_register_xmm2_register_xmm5 = e,\
	p##_register_xmm2_register_xmm6 = e,\
	p##_register_xmm2_register_xmm7 = e,\
	p##_register_xmm3_register_xmm0 = e,\
	p##_register_xmm3_register_xmm1 = e,\
	p##_register_xmm3_register_xmm2 = e,\
	p##_register_xmm3_register_xmm3 = e,\
	p##_register_xmm3_register_xmm4 = e,\
	p##_register_xmm3_register_xmm5 = e,\
	p##_register_xmm3_register_xmm6 = e,\
	p##_register_xmm3_register_xmm7 = e,\
	p##_register_xmm4_register_xmm0 = e,\
	p##_register_xmm4_register_xmm1 = e,\
	p##_register_xmm4_register_xmm2 = e,\
	p##_register_xmm4_register_xmm3 = e,\
	p##_register_xmm4_register_xmm4 = e,\
	p##_register_xmm4_register_xmm5 = e,\
	p##_register_xmm4_register_xmm6 = e,\
	p##_register_xmm4_register_xmm7 = e,\
	p##_register_xmm5_register_xmm0 = e,\
	p##_register_xmm5_register_xmm1 = e,\
	p##_register_xmm5_register_xmm2 = e,\
	p##_register_xmm5_register_xmm3 = e,\
	p##_register_xmm5_register_xmm4 = e,\
	p##_register_xmm5_register_xmm5 = e,\
	p##_register_xmm5_register_xmm6 = e,\
	p##_register_xmm5_register_xmm7 = e,\
	p##_register_xmm6_register_xmm0 = e,\
	p##_register_xmm6_register_xmm1 = e,\
	p##_register_xmm6_register_xmm2 = e,\
	p##_register_xmm6_register_xmm3 = e,\
	p##_register_xmm6_register_xmm4 = e,\
	p##_register_xmm6_register_xmm5 = e,\
	p##_register_xmm6_register_xmm6 = e,\
	p##_register_xmm6_register_xmm7 = e,\
	p##_register_xmm7_register_xmm0 = e,\
	p##_register_xmm7_register_xmm1 = e,\
	p##_register_xmm7_register_xmm2 = e,\
	p##_register_xmm7_register_xmm3 = e,\
	p##_register_xmm7_register_xmm4 = e,\
	p##_register_xmm7_register_xmm5 = e,\
	p##_register_xmm7_register_xmm6 = e,\
	p##_register_xmm7_register_xmm7 = e,\
	p##_offset_register_xmm0 = e,\
	p##_offset_register_xmm1 = e,\
	p##_offset_register_xmm2 = e,\
	p##_offset_register_xmm3 = e,\
	p##_offset_register_xmm4 = e,\
	p##_offset_register_xmm5 = e,\
	p##_offset_register_xmm6 = e,\
	p##_offset_register_xmm7 = e,\
	p##_scale_register_xmm0 = e,\
	p##_scale_register_xmm1 = e,\
	p##_scale_register_xmm2 = e,\
	p##_scale_register_xmm3 = e,\
	p##_scale_register_xmm4 = e,\
	p##_scale_register_xmm5 = e,\
	p##_scale_register_xmm6 = e,\
	p##_scale_register_xmm7 = e,
#define NT80(p, e)\
	p##_offset_register_xmm0 = e,\
	p##_offset_register_xmm1 = e,\
	p##_offset_register_xmm2 = e,\
	p##_offset_register_xmm3 = e,\
	p##_offset_register_xmm4 = e,\
	p##_offset_register_xmm5 = e,\
	p##_offset_register_xmm6 = e,\
	p##_offset_register_xmm7 = e,\
	p##_scale_register_xmm0 = e,\
	p##_scale_register_xmm1 = e,\
	p##_scale_register_xmm2 = e,\
	p##_scale_register_xmm3 = e,\
	p##_scale_register_xmm4 = e,\
	p##_scale_register_xmm5 = e,\
	p##_scale_register_xmm6 = e,\
	p##_scale_register_xmm7 = e,
#define NT81(p, e)\
	p##_register_xmm0_register_xmm0 = e,\
	p##_register_xmm0_register_xmm1 = e,\
	p##_register_xmm0_register_xmm2 = e,\
	p##_register_xmm0_register_xmm3 = e,\
	p##_register_xmm0_register_xmm4 = e,\
	p##_register_xmm0_register_xmm5 = e,\
	p##_register_xmm0_register_xmm6 = e,\
	p##_register_xmm0_register_xmm7 = e,\
	p##_register_xmm0_offset = e,\
	p##_register_xmm0_scale = e,\
	p##_register_xmm1_register_xmm0 = e,\
	p##_register_xmm1_register_xmm1 = e,\
	p##_register_xmm1_register_xmm2 = e,\
	p##_register_xmm1_register_xmm3 = e,\
	p##_register_xmm1_register_xmm4 = e,\
	p##_register_xmm1_register_xmm5 = e,\
	p##_register_xmm1_register_xmm6 = e,\
	p##_register_xmm1_register_xmm7 = e,\
	p##_register_xmm1_offset = e,\
	p##_register_xmm1_scale = e,\
	p##_register_xmm2_register_xmm0 = e,\
	p##_register_xmm2_register_xmm1 = e,\
	p##_register_xmm2_register_xmm2 = e,\
	p##_register_xmm2_register_xmm3 = e,\
	p##_register_xmm2_register_xmm4 = e,\
	p##_register_xmm2_register_xmm5 = e,\
	p##_register_xmm2_register_xmm6 = e,\
	p##_register_xmm2_register_xmm7 = e,\
	p##_register_xmm2_offset = e,\
	p##_register_xmm2_scale = e,\
	p##_register_xmm3_register_xmm0 = e,\
	p##_register_xmm3_register_xmm1 = e,\
	p##_register_xmm3_register_xmm2 = e,\
	p##_register_xmm3_register_xmm3 = e,\
	p##_register_xmm3_register_xmm4 = e,\
	p##_register_xmm3_register_xmm5 = e,\
	p##_register_xmm3_register_xmm6 = e,\
	p##_register_xmm3_register_xmm7 = e,\
	p##_register_xmm3_offset = e,\
	p##_register_xmm3_scale = e,\
	p##_register_xmm4_register_xmm0 = e,\
	p##_register_xmm4_register_xmm1 = e,\
	p##_register_xmm4_register_xmm2 = e,\
	p##_register_xmm4_register_xmm3 = e,\
	p##_register_xmm4_register_xmm4 = e,\
	p##_register_xmm4_register_xmm5 = e,\
	p##_register_xmm4_register_xmm6 = e,\
	p##_register_xmm4_register_xmm7 = e,\
	p##_register_xmm4_offset = e,\
	p##_register_xmm4_scale = e,\
	p##_register_xmm5_register_xmm0 = e,\
	p##_register_xmm5_register_xmm1 = e,\
	p##_register_xmm5_register_xmm2 = e,\
	p##_register_xmm5_register_xmm3 = e,\
	p##_register_xmm5_register_xmm4 = e,\
	p##_register_xmm5_register_xmm5 = e,\
	p##_register_xmm5_register_xmm6 = e,\
	p##_register_xmm5_register_xmm7 = e,\
	p##_register_xmm5_offset = e,\
	p##_register_xmm5_scale = e,\
	p##_register_xmm6_register_xmm0 = e,\
	p##_register_xmm6_register_xmm1 = e,\
	p##_register_xmm6_register_xmm2 = e,\
	p##_register_xmm6_register_xmm3 = e,\
	p##_register_xmm6_register_xmm4 = e,\
	p##_register_xmm6_register_xmm5 = e,\
	p##_register_xmm6_register_xmm6 = e,\
	p##_register_xmm6_register_xmm7 = e,\
	p##_register_xmm6_offset = e,\
	p##_register_xmm6_scale = e,\
	p##_register_xmm7_register_xmm0 = e,\
	p##_register_xmm7_register_xmm1 = e,\
	p##_register_xmm7_register_xmm2 = e,\
	p##_register_xmm7_register_xmm3 = e,\
	p##_register_xmm7_register_xmm4 = e,\
	p##_register_xmm7_register_xmm5 = e,\
	p##_register_xmm7_register_xmm6 = e,\
	p##_register_xmm7_register_xmm7 = e,\
	p##_register_xmm7_offset = e,\
	p##_register_xmm7_scale = e,
#define NT82(p, e)\
	p##_register_eax_register_ax = e,\
	p##_register_eax_register_cx = e,\
	p##_register_eax_register_dx = e,\
	p##_register_eax_register_bx = e,\
	p##_register_eax_register_sp = e,\
	p##_register_eax_register_bp = e,\
	p##_register_eax_register_si = e,\
	p##_register_eax_register_di = e,\
	p##_register_eax_offset = e,\
	p##_register_eax_scale = e,\
	p##_register_ecx_register_ax = e,\
	p##_register_ecx_register_cx = e,\
	p##_register_ecx_register_dx = e,\
	p##_register_ecx_register_bx = e,\
	p##_register_ecx_register_sp = e,\
	p##_register_ecx_register_bp = e,\
	p##_register_ecx_register_si = e,\
	p##_register_ecx_register_di = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_register_ax = e,\
	p##_register_edx_register_cx = e,\
	p##_register_edx_register_dx = e,\
	p##_register_edx_register_bx = e,\
	p##_register_edx_register_sp = e,\
	p##_register_edx_register_bp = e,\
	p##_register_edx_register_si = e,\
	p##_register_edx_register_di = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_register_ax = e,\
	p##_register_ebx_register_cx = e,\
	p##_register_ebx_register_dx = e,\
	p##_register_ebx_register_bx = e,\
	p##_register_ebx_register_sp = e,\
	p##_register_ebx_register_bp = e,\
	p##_register_ebx_register_si = e,\
	p##_register_ebx_register_di = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_register_ax = e,\
	p##_register_esp_register_cx = e,\
	p##_register_esp_register_dx = e,\
	p##_register_esp_register_bx = e,\
	p##_register_esp_register_sp = e,\
	p##_register_esp_register_bp = e,\
	p##_register_esp_register_si = e,\
	p##_register_esp_register_di = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_register_ax = e,\
	p##_register_ebp_register_cx = e,\
	p##_register_ebp_register_dx = e,\
	p##_register_ebp_register_bx = e,\
	p##_register_ebp_register_sp = e,\
	p##_register_ebp_register_bp = e,\
	p##_register_ebp_register_si = e,\
	p##_register_ebp_register_di = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_register_ax = e,\
	p##_register_esi_register_cx = e,\
	p##_register_esi_register_dx = e,\
	p##_register_esi_register_bx = e,\
	p##_register_esi_register_sp = e,\
	p##_register_esi_register_bp = e,\
	p##_register_esi_register_si = e,\
	p##_register_esi_register_di = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_register_ax = e,\
	p##_register_edi_register_cx = e,\
	p##_register_edi_register_dx = e,\
	p##_register_edi_register_bx = e,\
	p##_register_edi_register_sp = e,\
	p##_register_edi_register_bp = e,\
	p##_register_edi_register_si = e,\
	p##_register_edi_register_di = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT83(p, e)\
	p##_register_dx_register_eax = e,
#define NT84(p, e)\
	p##_register_dx_register_al = e,
#define NT85(p, e)\
	p##_register_eax_register_dx = e,
#define NT86(p, e)\
	p##_register_al_register_dx = e,
#define NT87(p, e)\
	p##_immediate_register_eax = e,
#define NT88(p, e)\
	p##_immediate_register_al = e,
#define NT89(p, e)\
	p##_immediate_immediate = e,
#define NT90(p, e)\
	p##_register_edi_immediate = e,
#define NT91(p, e)\
	p##_register_esi_immediate = e,
#define NT92(p, e)\
	p##_register_ebp_immediate = e,
#define NT93(p, e)\
	p##_register_esp_immediate = e,
#define NT94(p, e)\
	p##_register_ebx_immediate = e,
#define NT95(p, e)\
	p##_register_edx_immediate = e,
#define NT96(p, e)\
	p##_register_ecx_immediate = e,
#define NT97(p, e)\
	p##_register_bh_immediate = e,
#define NT98(p, e)\
	p##_register_dh_immediate = e,
#define NT99(p, e)\
	p##_register_ch_immediate = e,
#define NT100(p, e)\
	p##_register_ah_immediate = e,
#define NT101(p, e)\
	p##_register_bl_immediate = e,
#define NT102(p, e)\
	p##_register_dl_immediate = e,
#define NT103(p, e)\
	p##_register_cl_immediate = e,
#define NT104(p, e)\
	p##_register_eax_register_esdi = e,
#define NT105(p, e)\
	p##_register_al_register_esdi = e,
#define NT106(p, e)\
	p##_register_eax_register_dssi = e,
#define NT107(p, e)\
	p##_register_al_register_dssi = e,
#define NT108(p, e)\
	p##_register_esdi_register_eax = e,
#define NT109(p, e)\
	p##_register_esdi_register_al = e,
#define NT110(p, e)\
	p##_register_dssi_register_esdi = e,
#define NT111(p, e)\
	p##_register_esdi_register_dssi = e,
#define NT112(p, e)\
	p##_offset_register_eax = e,
#define NT113(p, e)\
	p##_register_eax_register_eax = e,\
	p##_register_eax_register_ecx = e,\
	p##_register_eax_register_edx = e,\
	p##_register_eax_register_ebx = e,\
	p##_register_eax_register_esp = e,\
	p##_register_eax_register_ebp = e,\
	p##_register_eax_register_esi = e,\
	p##_register_eax_register_edi = e,\
	p##_register_ecx_register_eax = e,\
	p##_register_ecx_register_ecx = e,\
	p##_register_ecx_register_edx = e,\
	p##_register_ecx_register_ebx = e,\
	p##_register_ecx_register_esp = e,\
	p##_register_ecx_register_ebp = e,\
	p##_register_ecx_register_esi = e,\
	p##_register_ecx_register_edi = e,\
	p##_register_edx_register_eax = e,\
	p##_register_edx_register_ecx = e,\
	p##_register_edx_register_edx = e,\
	p##_register_edx_register_ebx = e,\
	p##_register_edx_register_esp = e,\
	p##_register_edx_register_ebp = e,\
	p##_register_edx_register_esi = e,\
	p##_register_edx_register_edi = e,\
	p##_register_ebx_register_eax = e,\
	p##_register_ebx_register_ecx = e,\
	p##_register_ebx_register_edx = e,\
	p##_register_ebx_register_ebx = e,\
	p##_register_ebx_register_esp = e,\
	p##_register_ebx_register_ebp = e,\
	p##_register_ebx_register_esi = e,\
	p##_register_ebx_register_edi = e,\
	p##_register_esp_register_eax = e,\
	p##_register_esp_register_ecx = e,\
	p##_register_esp_register_edx = e,\
	p##_register_esp_register_ebx = e,\
	p##_register_esp_register_esp = e,\
	p##_register_esp_register_ebp = e,\
	p##_register_esp_register_esi = e,\
	p##_register_esp_register_edi = e,\
	p##_register_ebp_register_eax = e,\
	p##_register_ebp_register_ecx = e,\
	p##_register_ebp_register_edx = e,\
	p##_register_ebp_register_ebx = e,\
	p##_register_ebp_register_esp = e,\
	p##_register_ebp_register_ebp = e,\
	p##_register_ebp_register_esi = e,\
	p##_register_ebp_register_edi = e,\
	p##_register_esi_register_eax = e,\
	p##_register_esi_register_ecx = e,\
	p##_register_esi_register_edx = e,\
	p##_register_esi_register_ebx = e,\
	p##_register_esi_register_esp = e,\
	p##_register_esi_register_ebp = e,\
	p##_register_esi_register_esi = e,\
	p##_register_esi_register_edi = e,\
	p##_register_edi_register_eax = e,\
	p##_register_edi_register_ecx = e,\
	p##_register_edi_register_edx = e,\
	p##_register_edi_register_ebx = e,\
	p##_register_edi_register_esp = e,\
	p##_register_edi_register_ebp = e,\
	p##_register_edi_register_esi = e,\
	p##_register_edi_register_edi = e,\
	p##_offset_register_ecx = e,\
	p##_offset_register_edx = e,\
	p##_offset_register_ebx = e,\
	p##_offset_register_esp = e,\
	p##_offset_register_ebp = e,\
	p##_offset_register_esi = e,\
	p##_offset_register_edi = e,\
	p##_scale_register_eax = e,\
	p##_scale_register_ecx = e,\
	p##_scale_register_edx = e,\
	p##_scale_register_ebx = e,\
	p##_scale_register_esp = e,\
	p##_scale_register_ebp = e,\
	p##_scale_register_esi = e,\
	p##_scale_register_edi = e,
#define NT114(p, e)\
	p##_offset_register_al = e,
#define NT115(p, e)\
	p##_register_al_register_al = e,\
	p##_register_al_register_cl = e,\
	p##_register_al_register_dl = e,\
	p##_register_al_register_bl = e,\
	p##_register_al_register_ah = e,\
	p##_register_al_register_ch = e,\
	p##_register_al_register_dh = e,\
	p##_register_al_register_bh = e,\
	p##_register_cl_register_al = e,\
	p##_register_cl_register_cl = e,\
	p##_register_cl_register_dl = e,\
	p##_register_cl_register_bl = e,\
	p##_register_cl_register_ah = e,\
	p##_register_cl_register_ch = e,\
	p##_register_cl_register_dh = e,\
	p##_register_cl_register_bh = e,\
	p##_register_dl_register_al = e,\
	p##_register_dl_register_cl = e,\
	p##_register_dl_register_dl = e,\
	p##_register_dl_register_bl = e,\
	p##_register_dl_register_ah = e,\
	p##_register_dl_register_ch = e,\
	p##_register_dl_register_dh = e,\
	p##_register_dl_register_bh = e,\
	p##_register_bl_register_al = e,\
	p##_register_bl_register_cl = e,\
	p##_register_bl_register_dl = e,\
	p##_register_bl_register_bl = e,\
	p##_register_bl_register_ah = e,\
	p##_register_bl_register_ch = e,\
	p##_register_bl_register_dh = e,\
	p##_register_bl_register_bh = e,\
	p##_register_ah_register_al = e,\
	p##_register_ah_register_cl = e,\
	p##_register_ah_register_dl = e,\
	p##_register_ah_register_bl = e,\
	p##_register_ah_register_ah = e,\
	p##_register_ah_register_ch = e,\
	p##_register_ah_register_dh = e,\
	p##_register_ah_register_bh = e,\
	p##_register_ch_register_al = e,\
	p##_register_ch_register_cl = e,\
	p##_register_ch_register_dl = e,\
	p##_register_ch_register_bl = e,\
	p##_register_ch_register_ah = e,\
	p##_register_ch_register_ch = e,\
	p##_register_ch_register_dh = e,\
	p##_register_ch_register_bh = e,\
	p##_register_dh_register_al = e,\
	p##_register_dh_register_cl = e,\
	p##_register_dh_register_dl = e,\
	p##_register_dh_register_bl = e,\
	p##_register_dh_register_ah = e,\
	p##_register_dh_register_ch = e,\
	p##_register_dh_register_dh = e,\
	p##_register_dh_register_bh = e,\
	p##_register_bh_register_al = e,\
	p##_register_bh_register_cl = e,\
	p##_register_bh_register_dl = e,\
	p##_register_bh_register_bl = e,\
	p##_register_bh_register_ah = e,\
	p##_register_bh_register_ch = e,\
	p##_register_bh_register_dh = e,\
	p##_register_bh_register_bh = e,\
	p##_offset_register_cl = e,\
	p##_offset_register_dl = e,\
	p##_offset_register_bl = e,\
	p##_offset_register_ah = e,\
	p##_offset_register_ch = e,\
	p##_offset_register_dh = e,\
	p##_offset_register_bh = e,\
	p##_scale_register_al = e,\
	p##_scale_register_cl = e,\
	p##_scale_register_dl = e,\
	p##_scale_register_bl = e,\
	p##_scale_register_ah = e,\
	p##_scale_register_ch = e,\
	p##_scale_register_dh = e,\
	p##_scale_register_bh = e,
#define NT116(p, e)\
	p##_register_eax_offset = e,
#define NT117(p, e)\
	p##_register_eax_scale = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT118(p, e)\
	p##_register_al_offset = e,
#define NT119(p, e)\
	p##_register_cl_offset = e,\
	p##_register_cl_scale = e,\
	p##_register_dl_offset = e,\
	p##_register_dl_scale = e,\
	p##_register_bl_offset = e,\
	p##_register_bl_scale = e,\
	p##_register_ah_offset = e,\
	p##_register_ah_scale = e,\
	p##_register_ch_offset = e,\
	p##_register_ch_scale = e,\
	p##_register_dh_offset = e,\
	p##_register_dh_scale = e,\
	p##_register_bh_offset = e,\
	p##_register_bh_scale = e,
#define NT120(p, e)\
	p##_offset_immediate = e,
#define NT121(p, e)\
	p##_register_eax_register_edi = e,
#define NT122(p, e)\
	p##_register_eax_register_esi = e,
#define NT123(p, e)\
	p##_register_eax_register_ebp = e,
#define NT124(p, e)\
	p##_register_eax_register_esp = e,
#define NT125(p, e)\
	p##_register_eax_register_ebx = e,
#define NT126(p, e)\
	p##_register_eax_register_edx = e,
#define NT127(p, e)\
	p##_register_eax_register_ecx = e,
#define NT128(p, e)\
	p##_register_ecx_register_eax = e,\
	p##_register_ecx_register_edx = e,\
	p##_register_ecx_register_ebx = e,\
	p##_register_ecx_register_esp = e,\
	p##_register_ecx_register_ebp = e,\
	p##_register_ecx_register_esi = e,\
	p##_register_ecx_register_edi = e,\
	p##_register_edx_register_eax = e,\
	p##_register_edx_register_edx = e,\
	p##_register_edx_register_ebx = e,\
	p##_register_edx_register_esp = e,\
	p##_register_edx_register_ebp = e,\
	p##_register_edx_register_esi = e,\
	p##_register_edx_register_edi = e,\
	p##_register_ebx_register_eax = e,\
	p##_register_ebx_register_edx = e,\
	p##_register_ebx_register_ebx = e,\
	p##_register_ebx_register_esp = e,\
	p##_register_ebx_register_ebp = e,\
	p##_register_ebx_register_esi = e,\
	p##_register_ebx_register_edi = e,\
	p##_register_esp_register_eax = e,\
	p##_register_esp_register_edx = e,\
	p##_register_esp_register_ebx = e,\
	p##_register_esp_register_esp = e,\
	p##_register_esp_register_ebp = e,\
	p##_register_esp_register_esi = e,\
	p##_register_esp_register_edi = e,\
	p##_register_ebp_register_eax = e,\
	p##_register_ebp_register_edx = e,\
	p##_register_ebp_register_ebx = e,\
	p##_register_ebp_register_esp = e,\
	p##_register_ebp_register_ebp = e,\
	p##_register_ebp_register_esi = e,\
	p##_register_ebp_register_edi = e,\
	p##_register_esi_register_eax = e,\
	p##_register_esi_register_edx = e,\
	p##_register_esi_register_ebx = e,\
	p##_register_esi_register_esp = e,\
	p##_register_esi_register_ebp = e,\
	p##_register_esi_register_esi = e,\
	p##_register_esi_register_edi = e,\
	p##_register_edi_register_eax = e,\
	p##_register_edi_register_edx = e,\
	p##_register_edi_register_ebx = e,\
	p##_register_edi_register_esp = e,\
	p##_register_edi_register_ebp = e,\
	p##_register_edi_register_esi = e,\
	p##_register_edi_register_edi = e,\
	p##_offset_register_eax = e,\
	p##_offset_register_edx = e,\
	p##_offset_register_ebx = e,\
	p##_offset_register_esp = e,\
	p##_offset_register_ebp = e,\
	p##_offset_register_esi = e,\
	p##_offset_register_edi = e,\
	p##_scale_register_eax = e,\
	p##_scale_register_edx = e,\
	p##_scale_register_ebx = e,\
	p##_scale_register_esp = e,\
	p##_scale_register_ebp = e,\
	p##_scale_register_esi = e,\
	p##_scale_register_edi = e,
#define NT129(p, e)\
	p##_offset = e,\
	p##_scale = e,
#define NT130(p, e)\
	p##_register_es_register_ax = e,\
	p##_register_es_register_cx = e,\
	p##_register_es_register_dx = e,\
	p##_register_es_register_bx = e,\
	p##_register_es_register_sp = e,\
	p##_register_es_register_bp = e,\
	p##_register_es_register_si = e,\
	p##_register_es_register_di = e,\
	p##_register_es_offset = e,\
	p##_register_es_scale = e,\
	p##_register_cs_register_ax = e,\
	p##_register_cs_register_cx = e,\
	p##_register_cs_register_dx = e,\
	p##_register_cs_register_bx = e,\
	p##_register_cs_register_sp = e,\
	p##_register_cs_register_bp = e,\
	p##_register_cs_register_si = e,\
	p##_register_cs_register_di = e,\
	p##_register_cs_offset = e,\
	p##_register_cs_scale = e,\
	p##_register_ss_register_ax = e,\
	p##_register_ss_register_cx = e,\
	p##_register_ss_register_dx = e,\
	p##_register_ss_register_bx = e,\
	p##_register_ss_register_sp = e,\
	p##_register_ss_register_bp = e,\
	p##_register_ss_register_si = e,\
	p##_register_ss_register_di = e,\
	p##_register_ss_offset = e,\
	p##_register_ss_scale = e,\
	p##_register_ds_register_ax = e,\
	p##_register_ds_register_cx = e,\
	p##_register_ds_register_dx = e,\
	p##_register_ds_register_bx = e,\
	p##_register_ds_register_sp = e,\
	p##_register_ds_register_bp = e,\
	p##_register_ds_register_si = e,\
	p##_register_ds_register_di = e,\
	p##_register_ds_offset = e,\
	p##_register_ds_scale = e,\
	p##_register_fs_register_ax = e,\
	p##_register_fs_register_cx = e,\
	p##_register_fs_register_dx = e,\
	p##_register_fs_register_bx = e,\
	p##_register_fs_register_sp = e,\
	p##_register_fs_register_bp = e,\
	p##_register_fs_register_si = e,\
	p##_register_fs_register_di = e,\
	p##_register_fs_offset = e,\
	p##_register_fs_scale = e,\
	p##_register_gs_register_ax = e,\
	p##_register_gs_register_cx = e,\
	p##_register_gs_register_dx = e,\
	p##_register_gs_register_bx = e,\
	p##_register_gs_register_sp = e,\
	p##_register_gs_register_bp = e,\
	p##_register_gs_register_si = e,\
	p##_register_gs_register_di = e,\
	p##_register_gs_offset = e,\
	p##_register_gs_scale = e,
#define NT131(p, e)\
	p##_register_ax_register_es = e,\
	p##_register_ax_register_cs = e,\
	p##_register_ax_register_ss = e,\
	p##_register_ax_register_ds = e,\
	p##_register_ax_register_fs = e,\
	p##_register_ax_register_gs = e,\
	p##_register_cx_register_es = e,\
	p##_register_cx_register_cs = e,\
	p##_register_cx_register_ss = e,\
	p##_register_cx_register_ds = e,\
	p##_register_cx_register_fs = e,\
	p##_register_cx_register_gs = e,\
	p##_register_dx_register_es = e,\
	p##_register_dx_register_cs = e,\
	p##_register_dx_register_ss = e,\
	p##_register_dx_register_ds = e,\
	p##_register_dx_register_fs = e,\
	p##_register_dx_register_gs = e,\
	p##_register_bx_register_es = e,\
	p##_register_bx_register_cs = e,\
	p##_register_bx_register_ss = e,\
	p##_register_bx_register_ds = e,\
	p##_register_bx_register_fs = e,\
	p##_register_bx_register_gs = e,\
	p##_register_sp_register_es = e,\
	p##_register_sp_register_cs = e,\
	p##_register_sp_register_ss = e,\
	p##_register_sp_register_ds = e,\
	p##_register_sp_register_fs = e,\
	p##_register_sp_register_gs = e,\
	p##_register_bp_register_es = e,\
	p##_register_bp_register_cs = e,\
	p##_register_bp_register_ss = e,\
	p##_register_bp_register_ds = e,\
	p##_register_bp_register_fs = e,\
	p##_register_bp_register_gs = e,\
	p##_register_si_register_es = e,\
	p##_register_si_register_cs = e,\
	p##_register_si_register_ss = e,\
	p##_register_si_register_ds = e,\
	p##_register_si_register_fs = e,\
	p##_register_si_register_gs = e,\
	p##_register_di_register_es = e,\
	p##_register_di_register_cs = e,\
	p##_register_di_register_ss = e,\
	p##_register_di_register_ds = e,\
	p##_register_di_register_fs = e,\
	p##_register_di_register_gs = e,\
	p##_offset_register_es = e,\
	p##_offset_register_cs = e,\
	p##_offset_register_ss = e,\
	p##_offset_register_ds = e,\
	p##_offset_register_fs = e,\
	p##_offset_register_gs = e,\
	p##_scale_register_es = e,\
	p##_scale_register_cs = e,\
	p##_scale_register_ss = e,\
	p##_scale_register_ds = e,\
	p##_scale_register_fs = e,\
	p##_scale_register_gs = e,
#define NT132(p, e)\
	p##_relative = e,
#define NT133(p, e)\
	p##_register_dx_register_dssi = e,
#define NT134(p, e)\
	p##_register_esdi_register_dx = e,
#define NT135(p, e)\
	p##_register_eax_register_eax_immediate = e,\
	p##_register_eax_register_ecx_immediate = e,\
	p##_register_eax_register_edx_immediate = e,\
	p##_register_eax_register_ebx_immediate = e,\
	p##_register_eax_register_esp_immediate = e,\
	p##_register_eax_register_ebp_immediate = e,\
	p##_register_eax_register_esi_immediate = e,\
	p##_register_eax_register_edi_immediate = e,\
	p##_register_eax_offset_immediate = e,\
	p##_register_eax_scale_immediate = e,\
	p##_register_ecx_register_eax_immediate = e,\
	p##_register_ecx_register_ecx_immediate = e,\
	p##_register_ecx_register_edx_immediate = e,\
	p##_register_ecx_register_ebx_immediate = e,\
	p##_register_ecx_register_esp_immediate = e,\
	p##_register_ecx_register_ebp_immediate = e,\
	p##_register_ecx_register_esi_immediate = e,\
	p##_register_ecx_register_edi_immediate = e,\
	p##_register_ecx_offset_immediate = e,\
	p##_register_ecx_scale_immediate = e,\
	p##_register_edx_register_eax_immediate = e,\
	p##_register_edx_register_ecx_immediate = e,\
	p##_register_edx_register_edx_immediate = e,\
	p##_register_edx_register_ebx_immediate = e,\
	p##_register_edx_register_esp_immediate = e,\
	p##_register_edx_register_ebp_immediate = e,\
	p##_register_edx_register_esi_immediate = e,\
	p##_register_edx_register_edi_immediate = e,\
	p##_register_edx_offset_immediate = e,\
	p##_register_edx_scale_immediate = e,\
	p##_register_ebx_register_eax_immediate = e,\
	p##_register_ebx_register_ecx_immediate = e,\
	p##_register_ebx_register_edx_immediate = e,\
	p##_register_ebx_register_ebx_immediate = e,\
	p##_register_ebx_register_esp_immediate = e,\
	p##_register_ebx_register_ebp_immediate = e,\
	p##_register_ebx_register_esi_immediate = e,\
	p##_register_ebx_register_edi_immediate = e,\
	p##_register_ebx_offset_immediate = e,\
	p##_register_ebx_scale_immediate = e,\
	p##_register_esp_register_eax_immediate = e,\
	p##_register_esp_register_ecx_immediate = e,\
	p##_register_esp_register_edx_immediate = e,\
	p##_register_esp_register_ebx_immediate = e,\
	p##_register_esp_register_esp_immediate = e,\
	p##_register_esp_register_ebp_immediate = e,\
	p##_register_esp_register_esi_immediate = e,\
	p##_register_esp_register_edi_immediate = e,\
	p##_register_esp_offset_immediate = e,\
	p##_register_esp_scale_immediate = e,\
	p##_register_ebp_register_eax_immediate = e,\
	p##_register_ebp_register_ecx_immediate = e,\
	p##_register_ebp_register_edx_immediate = e,\
	p##_register_ebp_register_ebx_immediate = e,\
	p##_register_ebp_register_esp_immediate = e,\
	p##_register_ebp_register_ebp_immediate = e,\
	p##_register_ebp_register_esi_immediate = e,\
	p##_register_ebp_register_edi_immediate = e,\
	p##_register_ebp_offset_immediate = e,\
	p##_register_ebp_scale_immediate = e,\
	p##_register_esi_register_eax_immediate = e,\
	p##_register_esi_register_ecx_immediate = e,\
	p##_register_esi_register_edx_immediate = e,\
	p##_register_esi_register_ebx_immediate = e,\
	p##_register_esi_register_esp_immediate = e,\
	p##_register_esi_register_ebp_immediate = e,\
	p##_register_esi_register_esi_immediate = e,\
	p##_register_esi_register_edi_immediate = e,\
	p##_register_esi_offset_immediate = e,\
	p##_register_esi_scale_immediate = e,\
	p##_register_edi_register_eax_immediate = e,\
	p##_register_edi_register_ecx_immediate = e,\
	p##_register_edi_register_edx_immediate = e,\
	p##_register_edi_register_ebx_immediate = e,\
	p##_register_edi_register_esp_immediate = e,\
	p##_register_edi_register_ebp_immediate = e,\
	p##_register_edi_register_esi_immediate = e,\
	p##_register_edi_register_edi_immediate = e,\
	p##_register_edi_offset_immediate = e,\
	p##_register_edi_scale_immediate = e,
#define NT136(p, e)\
	p##_immediate = e,
#define NT137(p, e)\
	p##_register_ax_register_ax = e,\
	p##_register_ax_register_cx = e,\
	p##_register_ax_register_dx = e,\
	p##_register_ax_register_bx = e,\
	p##_register_ax_register_sp = e,\
	p##_register_ax_register_bp = e,\
	p##_register_ax_register_si = e,\
	p##_register_ax_register_di = e,\
	p##_register_cx_register_ax = e,\
	p##_register_cx_register_cx = e,\
	p##_register_cx_register_dx = e,\
	p##_register_cx_register_bx = e,\
	p##_register_cx_register_sp = e,\
	p##_register_cx_register_bp = e,\
	p##_register_cx_register_si = e,\
	p##_register_cx_register_di = e,\
	p##_register_dx_register_ax = e,\
	p##_register_dx_register_cx = e,\
	p##_register_dx_register_dx = e,\
	p##_register_dx_register_bx = e,\
	p##_register_dx_register_sp = e,\
	p##_register_dx_register_bp = e,\
	p##_register_dx_register_si = e,\
	p##_register_dx_register_di = e,\
	p##_register_bx_register_ax = e,\
	p##_register_bx_register_cx = e,\
	p##_register_bx_register_dx = e,\
	p##_register_bx_register_bx = e,\
	p##_register_bx_register_sp = e,\
	p##_register_bx_register_bp = e,\
	p##_register_bx_register_si = e,\
	p##_register_bx_register_di = e,\
	p##_register_sp_register_ax = e,\
	p##_register_sp_register_cx = e,\
	p##_register_sp_register_dx = e,\
	p##_register_sp_register_bx = e,\
	p##_register_sp_register_sp = e,\
	p##_register_sp_register_bp = e,\
	p##_register_sp_register_si = e,\
	p##_register_sp_register_di = e,\
	p##_register_bp_register_ax = e,\
	p##_register_bp_register_cx = e,\
	p##_register_bp_register_dx = e,\
	p##_register_bp_register_bx = e,\
	p##_register_bp_register_sp = e,\
	p##_register_bp_register_bp = e,\
	p##_register_bp_register_si = e,\
	p##_register_bp_register_di = e,\
	p##_register_si_register_ax = e,\
	p##_register_si_register_cx = e,\
	p##_register_si_register_dx = e,\
	p##_register_si_register_bx = e,\
	p##_register_si_register_sp = e,\
	p##_register_si_register_bp = e,\
	p##_register_si_register_si = e,\
	p##_register_si_register_di = e,\
	p##_register_di_register_ax = e,\
	p##_register_di_register_cx = e,\
	p##_register_di_register_dx = e,\
	p##_register_di_register_bx = e,\
	p##_register_di_register_sp = e,\
	p##_register_di_register_bp = e,\
	p##_register_di_register_si = e,\
	p##_register_di_register_di = e,\
	p##_offset_register_ax = e,\
	p##_offset_register_cx = e,\
	p##_offset_register_dx = e,\
	p##_offset_register_bx = e,\
	p##_offset_register_sp = e,\
	p##_offset_register_bp = e,\
	p##_offset_register_si = e,\
	p##_offset_register_di = e,\
	p##_scale_register_ax = e,\
	p##_scale_register_cx = e,\
	p##_scale_register_dx = e,\
	p##_scale_register_bx = e,\
	p##_scale_register_sp = e,\
	p##_scale_register_bp = e,\
	p##_scale_register_si = e,\
	p##_scale_register_di = e,
#define NT138(p, e)\
	p##_register_edi = e,
#define NT139(p, e)\
	p##_register_esi = e,
#define NT140(p, e)\
	p##_register_ebp = e,
#define NT141(p, e)\
	p##_register_esp = e,
#define NT142(p, e)\
	p##_register_ebx = e,
#define NT143(p, e)\
	p##_register_edx = e,
#define NT144(p, e)\
	p##_register_ecx = e,
#define NT145(p, e)\
	p##_register_eax = e,
#define NT146(p, e)\
	p##_register_ds = e,
#define NT147(p, e)\
	p##_register_ss = e,
#define NT148(p, e)\
	p##_register_cs = e,
#define NT149(p, e)\
	p##_register_es = e,
#define NT150(p, e)\
	p##_register_eax_immediate = e,
#define NT151(p, e)\
	p##_register_al_immediate = e,
#define NT152(p, e)\
	p##_register_eax_offset = e,\
	p##_register_eax_scale = e,\
	p##_register_ecx_offset = e,\
	p##_register_ecx_scale = e,\
	p##_register_edx_offset = e,\
	p##_register_edx_scale = e,\
	p##_register_ebx_offset = e,\
	p##_register_ebx_scale = e,\
	p##_register_esp_offset = e,\
	p##_register_esp_scale = e,\
	p##_register_ebp_offset = e,\
	p##_register_ebp_scale = e,\
	p##_register_esi_offset = e,\
	p##_register_esi_scale = e,\
	p##_register_edi_offset = e,\
	p##_register_edi_scale = e,
#define NT153(p, e)\
	p##_register_al_offset = e,\
	p##_register_al_scale = e,\
	p##_register_cl_offset = e,\
	p##_register_cl_scale = e,\
	p##_register_dl_offset = e,\
	p##_register_dl_scale = e,\
	p##_register_bl_offset = e,\
	p##_register_bl_scale = e,\
	p##_register_ah_offset = e,\
	p##_register_ah_scale = e,\
	p##_register_ch_offset = e,\
	p##_register_ch_scale = e,\
	p##_register_dh_offset = e,\
	p##_register_dh_scale = e,\
	p##_register_bh_offset = e,\
	p##_register_bh_scale = e,
#define NT154(p, e)\
	p##_register_eax_register_eax = e,\
	p##_register_eax_register_ecx = e,\
	p##_register_eax_register_edx = e,\
	p##_register_eax_register_ebx = e,\
	p##_register_eax_register_esp = e,\
	p##_register_eax_register_ebp = e,\
	p##_register_eax_register_esi = e,\
	p##_register_eax_register_edi = e,\
	p##_register_ecx_register_eax = e,\
	p##_register_ecx_register_ecx = e,\
	p##_register_ecx_register_edx = e,\
	p##_register_ecx_register_ebx = e,\
	p##_register_ecx_register_esp = e,\
	p##_register_ecx_register_ebp = e,\
	p##_register_ecx_register_esi = e,\
	p##_register_ecx_register_edi = e,\
	p##_register_edx_register_eax = e,\
	p##_register_edx_register_ecx = e,\
	p##_register_edx_register_edx = e,\
	p##_register_edx_register_ebx = e,\
	p##_register_edx_register_esp = e,\
	p##_register_edx_register_ebp = e,\
	p##_register_edx_register_esi = e,\
	p##_register_edx_register_edi = e,\
	p##_register_ebx_register_eax = e,\
	p##_register_ebx_register_ecx = e,\
	p##_register_ebx_register_edx = e,\
	p##_register_ebx_register_ebx = e,\
	p##_register_ebx_register_esp = e,\
	p##_register_ebx_register_ebp = e,\
	p##_register_ebx_register_esi = e,\
	p##_register_ebx_register_edi = e,\
	p##_register_esp_register_eax = e,\
	p##_register_esp_register_ecx = e,\
	p##_register_esp_register_edx = e,\
	p##_register_esp_register_ebx = e,\
	p##_register_esp_register_esp = e,\
	p##_register_esp_register_ebp = e,\
	p##_register_esp_register_esi = e,\
	p##_register_esp_register_edi = e,\
	p##_register_ebp_register_eax = e,\
	p##_register_ebp_register_ecx = e,\
	p##_register_ebp_register_edx = e,\
	p##_register_ebp_register_ebx = e,\
	p##_register_ebp_register_esp = e,\
	p##_register_ebp_register_ebp = e,\
	p##_register_ebp_register_esi = e,\
	p##_register_ebp_register_edi = e,\
	p##_register_esi_register_eax = e,\
	p##_register_esi_register_ecx = e,\
	p##_register_esi_register_edx = e,\
	p##_register_esi_register_ebx = e,\
	p##_register_esi_register_esp = e,\
	p##_register_esi_register_ebp = e,\
	p##_register_esi_register_esi = e,\
	p##_register_esi_register_edi = e,\
	p##_register_edi_register_eax = e,\
	p##_register_edi_register_ecx = e,\
	p##_register_edi_register_edx = e,\
	p##_register_edi_register_ebx = e,\
	p##_register_edi_register_esp = e,\
	p##_register_edi_register_ebp = e,\
	p##_register_edi_register_esi = e,\
	p##_register_edi_register_edi = e,\
	p##_offset_register_eax = e,\
	p##_offset_register_ecx = e,\
	p##_offset_register_edx = e,\
	p##_offset_register_ebx = e,\
	p##_offset_register_esp = e,\
	p##_offset_register_ebp = e,\
	p##_offset_register_esi = e,\
	p##_offset_register_edi = e,\
	p##_scale_register_eax = e,\
	p##_scale_register_ecx = e,\
	p##_scale_register_edx = e,\
	p##_scale_register_ebx = e,\
	p##_scale_register_esp = e,\
	p##_scale_register_ebp = e,\
	p##_scale_register_esi = e,\
	p##_scale_register_edi = e,
#define NT155(p, e)\
	p##_register_al_register_al = e,\
	p##_register_al_register_cl = e,\
	p##_register_al_register_dl = e,\
	p##_register_al_register_bl = e,\
	p##_register_al_register_ah = e,\
	p##_register_al_register_ch = e,\
	p##_register_al_register_dh = e,\
	p##_register_al_register_bh = e,\
	p##_register_cl_register_al = e,\
	p##_register_cl_register_cl = e,\
	p##_register_cl_register_dl = e,\
	p##_register_cl_register_bl = e,\
	p##_register_cl_register_ah = e,\
	p##_register_cl_register_ch = e,\
	p##_register_cl_register_dh = e,\
	p##_register_cl_register_bh = e,\
	p##_register_dl_register_al = e,\
	p##_register_dl_register_cl = e,\
	p##_register_dl_register_dl = e,\
	p##_register_dl_register_bl = e,\
	p##_register_dl_register_ah = e,\
	p##_register_dl_register_ch = e,\
	p##_register_dl_register_dh = e,\
	p##_register_dl_register_bh = e,\
	p##_register_bl_register_al = e,\
	p##_register_bl_register_cl = e,\
	p##_register_bl_register_dl = e,\
	p##_register_bl_register_bl = e,\
	p##_register_bl_register_ah = e,\
	p##_register_bl_register_ch = e,\
	p##_register_bl_register_dh = e,\
	p##_register_bl_register_bh = e,\
	p##_register_ah_register_al = e,\
	p##_register_ah_register_cl = e,\
	p##_register_ah_register_dl = e,\
	p##_register_ah_register_bl = e,\
	p##_register_ah_register_ah = e,\
	p##_register_ah_register_ch = e,\
	p##_register_ah_register_dh = e,\
	p##_register_ah_register_bh = e,\
	p##_register_ch_register_al = e,\
	p##_register_ch_register_cl = e,\
	p##_register_ch_register_dl = e,\
	p##_register_ch_register_bl = e,\
	p##_register_ch_register_ah = e,\
	p##_register_ch_register_ch = e,\
	p##_register_ch_register_dh = e,\
	p##_register_ch_register_bh = e,\
	p##_register_dh_register_al = e,\
	p##_register_dh_register_cl = e,\
	p##_register_dh_register_dl = e,\
	p##_register_dh_register_bl = e,\
	p##_register_dh_register_ah = e,\
	p##_register_dh_register_ch = e,\
	p##_register_dh_register_dh = e,\
	p##_register_dh_register_bh = e,\
	p##_register_bh_register_al = e,\
	p##_register_bh_register_cl = e,\
	p##_register_bh_register_dl = e,\
	p##_register_bh_register_bl = e,\
	p##_register_bh_register_ah = e,\
	p##_register_bh_register_ch = e,\
	p##_register_bh_register_dh = e,\
	p##_register_bh_register_bh = e,\
	p##_offset_register_al = e,\
	p##_offset_register_cl = e,\
	p##_offset_register_dl = e,\
	p##_offset_register_bl = e,\
	p##_offset_register_ah = e,\
	p##_offset_register_ch = e,\
	p##_offset_register_dh = e,\
	p##_offset_register_bh = e,\
	p##_scale_register_al = e,\
	p##_scale_register_cl = e,\
	p##_scale_register_dl = e,\
	p##_scale_register_bl = e,\
	p##_scale_register_ah = e,\
	p##_scale_register_ch = e,\
	p##_scale_register_dh = e,\
	p##_scale_register_bh = e,
enum _X86OpcodeLookupEntry {
	NT155(x86_opcode_addb,0)
	NT154(x86_opcode_addl,1)
	NT153(x86_opcode_addb,2)
	NT152(x86_opcode_addl,3)
	NT151(x86_opcode_addb,4)
	NT150(x86_opcode_addl,5)
	NT149(x86_opcode_pushl,6)
	NT149(x86_opcode_popl,7)
	NT155(x86_opcode_orb,8)
	NT154(x86_opcode_orl,9)
	NT153(x86_opcode_orb,10)
	NT152(x86_opcode_orl,11)
	NT151(x86_opcode_orb,12)
	NT150(x86_opcode_orl,13)
	NT148(x86_opcode_pushl,14)
	NT155(x86_opcode_adcb,15)
	NT154(x86_opcode_adcl,16)
	NT153(x86_opcode_adcb,17)
	NT152(x86_opcode_adcl,18)
	NT151(x86_opcode_adcb,19)
	NT150(x86_opcode_adcl,20)
	NT147(x86_opcode_pushl,21)
	NT147(x86_opcode_popl,22)
	NT155(x86_opcode_sbbb,23)
	NT154(x86_opcode_sbbl,24)
	NT153(x86_opcode_sbbb,25)
	NT152(x86_opcode_sbbl,26)
	NT151(x86_opcode_sbbb,27)
	NT150(x86_opcode_sbbl,28)
	NT146(x86_opcode_pushl,29)
	NT146(x86_opcode_popl,30)
	NT155(x86_opcode_andb,31)
	NT154(x86_opcode_andl,32)
	NT153(x86_opcode_andb,33)
	NT152(x86_opcode_andl,34)
	NT151(x86_opcode_andb,35)
	NT150(x86_opcode_andl,36)
	NT0(x86_opcode_daa,37)
	NT155(x86_opcode_subb,38)
	NT154(x86_opcode_subl,39)
	NT153(x86_opcode_subb,40)
	NT152(x86_opcode_subl,41)
	NT151(x86_opcode_subb,42)
	NT150(x86_opcode_subl,43)
	NT0(x86_opcode_das,44)
	NT155(x86_opcode_xorb,45)
	NT154(x86_opcode_xorl,46)
	NT153(x86_opcode_xorb,47)
	NT152(x86_opcode_xorl,48)
	NT151(x86_opcode_xorb,49)
	NT150(x86_opcode_xorl,50)
	NT0(x86_opcode_aaa,51)
	NT155(x86_opcode_cmpb,52)
	NT154(x86_opcode_cmpl,53)
	NT153(x86_opcode_cmpb,54)
	NT152(x86_opcode_cmpl,55)
	NT151(x86_opcode_cmpb,56)
	NT150(x86_opcode_cmpl,57)
	NT0(x86_opcode_aas,58)
	NT145(x86_opcode_incl,59)
	NT144(x86_opcode_incl,60)
	NT143(x86_opcode_incl,61)
	NT142(x86_opcode_incl,62)
	NT141(x86_opcode_incl,63)
	NT140(x86_opcode_incl,64)
	NT139(x86_opcode_incl,65)
	NT138(x86_opcode_incl,66)
	NT145(x86_opcode_decl,67)
	NT144(x86_opcode_decl,68)
	NT143(x86_opcode_decl,69)
	NT142(x86_opcode_decl,70)
	NT141(x86_opcode_decl,71)
	NT140(x86_opcode_decl,72)
	NT139(x86_opcode_decl,73)
	NT138(x86_opcode_decl,74)
	NT145(x86_opcode_pushl,75)
	NT144(x86_opcode_pushl,76)
	NT143(x86_opcode_pushl,77)
	NT142(x86_opcode_pushl,78)
	NT141(x86_opcode_pushl,79)
	NT140(x86_opcode_pushl,80)
	NT139(x86_opcode_pushl,81)
	NT138(x86_opcode_pushl,82)
	NT145(x86_opcode_popl,83)
	NT144(x86_opcode_popl,84)
	NT143(x86_opcode_popl,85)
	NT142(x86_opcode_popl,86)
	NT141(x86_opcode_popl,87)
	NT140(x86_opcode_popl,88)
	NT139(x86_opcode_popl,89)
	NT138(x86_opcode_popl,90)
	NT0(x86_opcode_pusha,91)
	NT0(x86_opcode_popa,92)
	NT152(x86_opcode_boundl,93)
	NT137(x86_opcode_arplw,94)
	NT0(x86_opcode_opdsize,95)
	NT0(x86_opcode_addrsize,96)
	NT136(x86_opcode_pushl,97)
	NT135(x86_opcode_imull,98)
	NT136(x86_opcode_pushb,99)
	NT135(x86_opcode_imul,100)
	NT134(x86_opcode_insbw,101)
	NT134(x86_opcode_insdw,102)
	NT133(x86_opcode_outsbw,103)
	NT133(x86_opcode_outsdw,104)
	NT132(x86_opcode_job,105)
	NT132(x86_opcode_jnob,106)
	NT132(x86_opcode_jbb,107)
	NT132(x86_opcode_jnbb,108)
	NT132(x86_opcode_jzb,109)
	NT132(x86_opcode_jnzb,110)
	NT132(x86_opcode_jbeb,111)
	NT132(x86_opcode_jnbeb,112)
	NT132(x86_opcode_jsb,113)
	NT132(x86_opcode_jnsb,114)
	NT132(x86_opcode_jpb,115)
	NT132(x86_opcode_jnpb,116)
	NT132(x86_opcode_jlb,117)
	NT132(x86_opcode_jnlb,118)
	NT132(x86_opcode_jleb,119)
	NT132(x86_opcode_jnleb,120)
	NT155(x86_opcode_testb,121)
	NT154(x86_opcode_testl,122)
	NT155(x86_opcode_xchgb,123)
	NT128(x86_opcode_xchgl,124)
	NT115(x86_opcode_movb,125)
	NT113(x86_opcode_movl,126)
	NT119(x86_opcode_movb,127)
	NT117(x86_opcode_movl,128)
	NT131(x86_opcode_movw,129)
	NT152(x86_opcode_leal,130)
	NT130(x86_opcode_movw,131)
	NT129(x86_opcode_popl,132)
	NT0(x86_opcode_nop,133)
	NT127(x86_opcode_xchgl,134)
	NT126(x86_opcode_xchgl,135)
	NT125(x86_opcode_xchgl,136)
	NT124(x86_opcode_xchgl,137)
	NT123(x86_opcode_xchgl,138)
	NT122(x86_opcode_xchgl,139)
	NT121(x86_opcode_xchgl,140)
	NT0(x86_opcode_cbw,141)
	NT0(x86_opcode_cwd,142)
	NT120(x86_opcode_lcall,143)
	NT0(x86_opcode_fwait,144)
	NT0(x86_opcode_pushf,145)
	NT0(x86_opcode_popf,146)
	NT0(x86_opcode_sahf,147)
	NT0(x86_opcode_lahf,148)
	NT118(x86_opcode_movb,149)
	NT116(x86_opcode_movl,150)
	NT114(x86_opcode_movb,151)
	NT112(x86_opcode_movl,152)
	NT111(x86_opcode_movsb,153)
	NT111(x86_opcode_movsd,154)
	NT111(x86_opcode_cmpsb,155)
	NT110(x86_opcode_cmpsd,156)
	NT151(x86_opcode_testb,157)
	NT150(x86_opcode_testl,158)
	NT109(x86_opcode_stosbb,159)
	NT108(x86_opcode_stosdl,160)
	NT107(x86_opcode_lodsbb,161)
	NT106(x86_opcode_lodsdl,162)
	NT105(x86_opcode_scasbb,163)
	NT104(x86_opcode_scasdl,164)
	NT151(x86_opcode_movb,165)
	NT103(x86_opcode_movb,166)
	NT102(x86_opcode_movb,167)
	NT101(x86_opcode_movb,168)
	NT100(x86_opcode_movb,169)
	NT99(x86_opcode_movb,170)
	NT98(x86_opcode_movb,171)
	NT97(x86_opcode_movb,172)
	NT150(x86_opcode_movl,173)
	NT96(x86_opcode_movl,174)
	NT95(x86_opcode_movl,175)
	NT94(x86_opcode_movl,176)
	NT93(x86_opcode_movl,177)
	NT92(x86_opcode_movl,178)
	NT91(x86_opcode_movl,179)
	NT90(x86_opcode_movl,180)
	NT136(x86_opcode_retw,181)
	NT0(x86_opcode_ret,182)
	NT152(x86_opcode_lesl,183)
	NT152(x86_opcode_ldsl,184)
	NT89(x86_opcode_enter,185)
	NT0(x86_opcode_leave,186)
	NT136(x86_opcode_retfw,187)
	NT0(x86_opcode_retf,188)
	NT0(x86_opcode_int3,189)
	NT136(x86_opcode_intb,190)
	NT0(x86_opcode_into,191)
	NT0(x86_opcode_iret,192)
	NT136(x86_opcode_aamb,193)
	NT136(x86_opcode_aadb,194)
	NT0(x86_opcode_xlat,195)
	NT132(x86_opcode_loopneb,196)
	NT132(x86_opcode_loopeb,197)
	NT132(x86_opcode_loopb,198)
	NT132(x86_opcode_jcxzb,199)
	NT151(x86_opcode_inb,200)
	NT150(x86_opcode_in,201)
	NT88(x86_opcode_outb,202)
	NT87(x86_opcode_out,203)
	NT132(x86_opcode_calll,204)
	NT132(x86_opcode_jmpl,205)
	NT120(x86_opcode_ljmp,206)
	NT132(x86_opcode_jmpb,207)
	NT86(x86_opcode_in,208)
	NT85(x86_opcode_in,209)
	NT84(x86_opcode_out,210)
	NT83(x86_opcode_out,211)
	NT0(x86_opcode_lock,212)
	NT0(x86_opcode_repne,213)
	NT0(x86_opcode_rep,214)
	NT0(x86_opcode_hlt,215)
	NT0(x86_opcode_cmc,216)
	NT0(x86_opcode_clc,217)
	NT0(x86_opcode_stc,218)
	NT0(x86_opcode_cli,219)
	NT0(x86_opcode_sti,220)
	NT0(x86_opcode_cld,221)
	NT0(x86_opcode_std,222)
	NT82(x86_opcode_lar,223)
	NT82(x86_opcode_lsl,224)
	NT0(x86_opcode_clts,225)
	NT0(x86_opcode_invd,226)
	NT0(x86_opcode_wbinvd,227)
	NT0(x86_opcode_ud2a,228)
	NT81(x86_opcode_movups,229)
	NT80(x86_opcode_movups,230)
	NT79(x86_opcode_movlpsl,231)
	NT78(x86_opcode_movlpsl,232)
	NT81(x86_opcode_unpcklps,233)
	NT81(x86_opcode_unpckhps,234)
	NT81(x86_opcode_movhpsl,235)
	NT80(x86_opcode_movhpsl,236)
	NT77(x86_opcode_movl,237)
	NT76(x86_opcode_movl,238)
	NT75(x86_opcode_movl,239)
	NT74(x86_opcode_movl,240)
	NT81(x86_opcode_movaps,241)
	NT80(x86_opcode_movaps,242)
	NT73(x86_opcode_cvtpi2ps,243)
	NT79(x86_opcode_movntps,244)
	NT72(x86_opcode_cvttps2pi,245)
	NT72(x86_opcode_cvtps2pi,246)
	NT81(x86_opcode_ucomissl,247)
	NT81(x86_opcode_comiss,248)
	NT0(x86_opcode_wrmsr,249)
	NT0(x86_opcode_rdtsc,250)
	NT0(x86_opcode_rdmsr,251)
	NT0(x86_opcode_rdpmc,252)
	NT0(x86_opcode_sysenter,253)
	NT0(x86_opcode_sysexit,254)
	NT71(x86_opcode_movntil,255)
	NT71(x86_opcode_cmovol,256)
	NT71(x86_opcode_cmovnol,257)
	NT71(x86_opcode_cmovbl,258)
	NT71(x86_opcode_cmovael,259)
	NT71(x86_opcode_cmovel,260)
	NT71(x86_opcode_cmovnel,261)
	NT71(x86_opcode_cmovbel,262)
	NT71(x86_opcode_cmoval,263)
	NT71(x86_opcode_cmovsl,264)
	NT71(x86_opcode_cmovnsl,265)
	NT71(x86_opcode_cmovpl,266)
	NT71(x86_opcode_cmovnpl,267)
	NT71(x86_opcode_cmovll,268)
	NT71(x86_opcode_cmovnll,269)
	NT71(x86_opcode_cmovlel,270)
	NT71(x86_opcode_cmovnlel,271)
	NT70(x86_opcode_movmskps,272)
	NT81(x86_opcode_sqrtps,273)
	NT81(x86_opcode_rsqrtps,274)
	NT81(x86_opcode_rcpps,275)
	NT81(x86_opcode_andps,276)
	NT81(x86_opcode_andnps,277)
	NT81(x86_opcode_orps,278)
	NT81(x86_opcode_xorps,279)
	NT81(x86_opcode_addps,280)
	NT81(x86_opcode_mulps,281)
	NT81(x86_opcode_cvtps2pd,282)
	NT81(x86_opcode_cvtdq2ps,283)
	NT81(x86_opcode_subps,284)
	NT81(x86_opcode_minps,285)
	NT81(x86_opcode_divps,286)
	NT81(x86_opcode_maxps,287)
	NT69(x86_opcode_punpcklbwl,288)
	NT69(x86_opcode_punpcklwdl,289)
	NT69(x86_opcode_punpckldql,290)
	NT69(x86_opcode_packsswbl,291)
	NT69(x86_opcode_pcmpgtbl,292)
	NT69(x86_opcode_pcmpgtwl,293)
	NT69(x86_opcode_pcmpgtdl,294)
	NT69(x86_opcode_packuswbl,295)
	NT69(x86_opcode_punpckhbwl,296)
	NT69(x86_opcode_punpckhwdl,297)
	NT69(x86_opcode_punpckhdql,298)
	NT69(x86_opcode_packssdwl,299)
	NT68(x86_opcode_movdl,300)
	NT69(x86_opcode_movql,301)
	NT67(x86_opcode_pshufw,302)
	NT69(x86_opcode_pcmpeqbl,303)
	NT69(x86_opcode_pcmpeqwl,304)
	NT69(x86_opcode_pcmpeqdl,305)
	NT0(x86_opcode_emms,306)
	NT66(x86_opcode_movdl,307)
	NT65(x86_opcode_movql,308)
	NT132(x86_opcode_jol,309)
	NT132(x86_opcode_jnol,310)
	NT132(x86_opcode_jbl,311)
	NT132(x86_opcode_jael,312)
	NT132(x86_opcode_jel,313)
	NT132(x86_opcode_jnel,314)
	NT132(x86_opcode_jbel,315)
	NT132(x86_opcode_jal,316)
	NT132(x86_opcode_jsl,317)
	NT132(x86_opcode_jnsl,318)
	NT132(x86_opcode_jpl,319)
	NT132(x86_opcode_jnpl,320)
	NT132(x86_opcode_jll,321)
	NT132(x86_opcode_jnll,322)
	NT132(x86_opcode_jlel,323)
	NT132(x86_opcode_jnlel,324)
	NT64(x86_opcode_setob,325)
	NT64(x86_opcode_setnob,326)
	NT64(x86_opcode_setbb,327)
	NT64(x86_opcode_setaeb,328)
	NT64(x86_opcode_seteb,329)
	NT64(x86_opcode_setneb,330)
	NT64(x86_opcode_setbeb,331)
	NT64(x86_opcode_setab,332)
	NT64(x86_opcode_setsb,333)
	NT64(x86_opcode_setnsb,334)
	NT64(x86_opcode_setpb,335)
	NT64(x86_opcode_setnpb,336)
	NT64(x86_opcode_setlb,337)
	NT64(x86_opcode_setnlb,338)
	NT64(x86_opcode_setleb,339)
	NT64(x86_opcode_setnleb,340)
	NT63(x86_opcode_pushl,341)
	NT63(x86_opcode_popl,342)
	NT0(x86_opcode_cpuid,343)
	NT154(x86_opcode_btl,344)
	NT62(x86_opcode_shld,345)
	NT61(x86_opcode_shld,346)
	NT60(x86_opcode_pushl,347)
	NT60(x86_opcode_popl,348)
	NT0(x86_opcode_rsm,349)
	NT154(x86_opcode_btsl,350)
	NT62(x86_opcode_shrd,351)
	NT61(x86_opcode_shrd,352)
	NT71(x86_opcode_imull,353)
	NT155(x86_opcode_cmpxchgb,354)
	NT154(x86_opcode_cmpxchgl,355)
	NT59(x86_opcode_lssl,356)
	NT154(x86_opcode_btrl,357)
	NT59(x86_opcode_lfsl,358)
	NT59(x86_opcode_lgsl,359)
	NT58(x86_opcode_movzx,360)
	NT82(x86_opcode_movzxw,361)
	NT154(x86_opcode_btcl,362)
	NT71(x86_opcode_bsfl,363)
	NT71(x86_opcode_bsrl,364)
	NT58(x86_opcode_movsx,365)
	NT82(x86_opcode_movsxw,366)
	NT155(x86_opcode_xaddb,367)
	NT154(x86_opcode_xaddl,368)
	NT57(x86_opcode_cmpps,369)
	NT56(x86_opcode_movntil,370)
	NT55(x86_opcode_pinsrw,371)
	NT54(x86_opcode_pextrw,372)
	NT57(x86_opcode_shufps,373)
	NT53(x86_opcode_bswapl,374)
	NT52(x86_opcode_bswapl,375)
	NT51(x86_opcode_bswapl,376)
	NT50(x86_opcode_bswapl,377)
	NT49(x86_opcode_bswapl,378)
	NT48(x86_opcode_bswapl,379)
	NT47(x86_opcode_bswapl,380)
	NT46(x86_opcode_bswapl,381)
	NT69(x86_opcode_psrlwl,382)
	NT69(x86_opcode_psrldl,383)
	NT69(x86_opcode_psrlql,384)
	NT69(x86_opcode_paddql,385)
	NT69(x86_opcode_pmullwl,386)
	NT45(x86_opcode_pmovmskbl,387)
	NT69(x86_opcode_psubusbl,388)
	NT69(x86_opcode_psubuswl,389)
	NT69(x86_opcode_pminubl,390)
	NT69(x86_opcode_pandl,391)
	NT69(x86_opcode_paddusbl,392)
	NT69(x86_opcode_padduswl,393)
	NT69(x86_opcode_pmaxubl,394)
	NT69(x86_opcode_pandnl,395)
	NT69(x86_opcode_pavgbl,396)
	NT69(x86_opcode_psrawl,397)
	NT69(x86_opcode_psradl,398)
	NT69(x86_opcode_pavgwl,399)
	NT69(x86_opcode_pmulhuwl,400)
	NT69(x86_opcode_pmulhwl,401)
	NT79(x86_opcode_movntql,402)
	NT69(x86_opcode_psubsbl,403)
	NT69(x86_opcode_psubswl,404)
	NT69(x86_opcode_pminswl,405)
	NT69(x86_opcode_porl,406)
	NT69(x86_opcode_paddsbl,407)
	NT69(x86_opcode_paddswl,408)
	NT69(x86_opcode_pmaxswl,409)
	NT69(x86_opcode_pxorl,410)
	NT69(x86_opcode_psllwl,411)
	NT69(x86_opcode_pslldl,412)
	NT69(x86_opcode_psllql,413)
	NT69(x86_opcode_pmuludql,414)
	NT69(x86_opcode_pmaddwdl,415)
	NT69(x86_opcode_psadbwl,416)
	NT69(x86_opcode_maskmovq,417)
	NT69(x86_opcode_psubbl,418)
	NT69(x86_opcode_psubwl,419)
	NT69(x86_opcode_psubdl,420)
	NT69(x86_opcode_psubql,421)
	NT69(x86_opcode_paddbl,422)
	NT69(x86_opcode_paddwl,423)
	NT69(x86_opcode_padddl,424)
	NT81(x86_opcode_movupd,425)
	NT80(x86_opcode_movupd,426)
	NT81(x86_opcode_movlpdl,427)
	NT81(x86_opcode_unpcklpd,429)
	NT81(x86_opcode_unpckhpd,430)
	NT81(x86_opcode_movhpdl,431)
	NT80(x86_opcode_movhpdl,432)
	NT81(x86_opcode_movapd,433)
	NT80(x86_opcode_movapd,434)
	NT73(x86_opcode_cvtpi2pd,435)
	NT79(x86_opcode_movntpd,436)
	NT72(x86_opcode_cvttpd2pi,437)
	NT72(x86_opcode_cvtpd2pi,438)
	NT81(x86_opcode_ucomisd,439)
	NT81(x86_opcode_comisd,440)
	NT70(x86_opcode_movmskpd,441)
	NT81(x86_opcode_sqrtpd,442)
	NT81(x86_opcode_andpd,443)
	NT81(x86_opcode_andnpd,444)
	NT81(x86_opcode_orpd,445)
	NT81(x86_opcode_xorpd,446)
	NT81(x86_opcode_addpd,447)
	NT81(x86_opcode_mulpd,448)
	NT81(x86_opcode_cvtpd2ps,449)
	NT81(x86_opcode_cvtps2dq,450)
	NT81(x86_opcode_subpd,451)
	NT81(x86_opcode_minpd,452)
	NT81(x86_opcode_divpd,453)
	NT81(x86_opcode_maxpd,454)
	NT81(x86_opcode_punpcklbwl,455)
	NT81(x86_opcode_punpcklwdl,456)
	NT81(x86_opcode_punpckldql,457)
	NT81(x86_opcode_packsswbl,458)
	NT81(x86_opcode_pcmpgtbl,459)
	NT81(x86_opcode_pcmpgtwl,460)
	NT81(x86_opcode_pcmpgtdl,461)
	NT81(x86_opcode_packuswbl,462)
	NT81(x86_opcode_punpcklqdql,467)
	NT81(x86_opcode_punpckhqdql,468)
	NT44(x86_opcode_movdl,469)
	NT81(x86_opcode_movdqal,470)
	NT57(x86_opcode_pshufd,471)
	NT81(x86_opcode_pcmpeqbl,472)
	NT81(x86_opcode_pcmpeqwl,473)
	NT81(x86_opcode_pcmpeqdl,474)
	NT70(x86_opcode_movdl,475)
	NT80(x86_opcode_movdqal,476)
	NT57(x86_opcode_cmppd,477)
	NT43(x86_opcode_pinsrw,478)
	NT42(x86_opcode_pextrw,479)
	NT57(x86_opcode_shufpd,480)
	NT81(x86_opcode_psrlwl,481)
	NT81(x86_opcode_psrldl,482)
	NT81(x86_opcode_psrlql,483)
	NT81(x86_opcode_paddql,484)
	NT81(x86_opcode_pmullwl,485)
	NT79(x86_opcode_movql,486)
	NT41(x86_opcode_pmovmskbl,487)
	NT81(x86_opcode_psubusbl,488)
	NT81(x86_opcode_psubuswl,489)
	NT81(x86_opcode_pminubl,490)
	NT81(x86_opcode_pandl,491)
	NT81(x86_opcode_paddusbl,492)
	NT81(x86_opcode_padduswl,493)
	NT81(x86_opcode_pmaxubl,494)
	NT81(x86_opcode_pandnl,495)
	NT81(x86_opcode_pavgbl,496)
	NT81(x86_opcode_psrawl,497)
	NT81(x86_opcode_psradl,498)
	NT81(x86_opcode_pavgwl,499)
	NT81(x86_opcode_pmulhuwl,500)
	NT81(x86_opcode_pmulhwl,501)
	NT81(x86_opcode_cvttpd2dq,502)
	NT79(x86_opcode_movntdql,503)
	NT81(x86_opcode_psubsbl,504)
	NT81(x86_opcode_psubswl,505)
	NT81(x86_opcode_pminswl,506)
	NT81(x86_opcode_porl,507)
	NT81(x86_opcode_paddsbl,508)
	NT81(x86_opcode_paddswl,509)
	NT81(x86_opcode_pmaxswl,510)
	NT81(x86_opcode_pxorl,511)
	NT81(x86_opcode_psllwl,512)
	NT81(x86_opcode_pslldl,513)
	NT81(x86_opcode_psllql,514)
	NT81(x86_opcode_pmuludql,515)
	NT81(x86_opcode_pmaddwdl,516)
	NT81(x86_opcode_psadbwl,517)
	NT81(x86_opcode_maskmovdqul,518)
	NT81(x86_opcode_psubbl,519)
	NT81(x86_opcode_psubwl,520)
	NT81(x86_opcode_psubdl,521)
	NT81(x86_opcode_psubql,522)
	NT81(x86_opcode_paddbl,523)
	NT81(x86_opcode_paddwl,524)
	NT81(x86_opcode_padddl,525)
	NT81(x86_opcode_movsd,526)
	NT80(x86_opcode_movsd,527)
	NT44(x86_opcode_cvtsi2sd,528)
	NT40(x86_opcode_cvttsd2si,529)
	NT40(x86_opcode_cvtsd2si,530)
	NT81(x86_opcode_sqrtsd,531)
	NT81(x86_opcode_addsd,532)
	NT81(x86_opcode_mulsd,533)
	NT81(x86_opcode_cvtsd2ss,534)
	NT81(x86_opcode_subsd,535)
	NT81(x86_opcode_minsd,536)
	NT81(x86_opcode_divsd,537)
	NT81(x86_opcode_maxsd,538)
	NT57(x86_opcode_pshuflw,539)
	NT57(x86_opcode_cmpsd,540)
	NT39(x86_opcode_movdq2ql,541)
	NT81(x86_opcode_cvtpd2dq,542)
	NT81(x86_opcode_movssl,543)
	NT80(x86_opcode_movssl,544)
	NT44(x86_opcode_cvtsi2ssl,545)
	NT40(x86_opcode_cvttss2sil,546)
	NT40(x86_opcode_cvtss2sil,547)
	NT81(x86_opcode_sqrtssl,548)
	NT81(x86_opcode_rsqrtssl,549)
	NT81(x86_opcode_rcpssl,550)
	NT81(x86_opcode_addssl,551)
	NT81(x86_opcode_mulssl,552)
	NT81(x86_opcode_cvtss2sdl,553)
	NT81(x86_opcode_cvttps2dq,554)
	NT81(x86_opcode_subssl,555)
	NT81(x86_opcode_minssl,556)
	NT81(x86_opcode_divssl,557)
	NT81(x86_opcode_maxssl,558)
	NT81(x86_opcode_movdqul,559)
	NT57(x86_opcode_pshufhw,560)
	NT78(x86_opcode_movql,561)
	NT80(x86_opcode_movdqul,562)
	NT57(x86_opcode_cmpss,563)
	NT73(x86_opcode_movq2dql,564)
	NT81(x86_opcode_cvtdq2pd,565)
	NT59(x86_opcode_fadds,566)
	NT59(x86_opcode_fmuls,567)
	NT59(x86_opcode_fcoms,568)
	NT59(x86_opcode_fcomps,569)
	NT59(x86_opcode_fsubs,570)
	NT59(x86_opcode_fsubrs,571)
	NT59(x86_opcode_fdivs,572)
	NT59(x86_opcode_fdivrs,573)
	NT38(x86_opcode_fadd,574)
	NT37(x86_opcode_fadd,575)
	NT36(x86_opcode_fadd,576)
	NT35(x86_opcode_fadd,577)
	NT34(x86_opcode_fadd,578)
	NT33(x86_opcode_fadd,579)
	NT32(x86_opcode_fadd,580)
	NT31(x86_opcode_fadd,581)
	NT38(x86_opcode_fmul,582)
	NT37(x86_opcode_fmul,583)
	NT36(x86_opcode_fmul,584)
	NT35(x86_opcode_fmul,585)
	NT34(x86_opcode_fmul,586)
	NT33(x86_opcode_fmul,587)
	NT32(x86_opcode_fmul,588)
	NT31(x86_opcode_fmul,589)
	NT38(x86_opcode_fcom,590)
	NT37(x86_opcode_fcom,591)
	NT36(x86_opcode_fcom,592)
	NT35(x86_opcode_fcom,593)
	NT34(x86_opcode_fcom,594)
	NT33(x86_opcode_fcom,595)
	NT32(x86_opcode_fcom,596)
	NT31(x86_opcode_fcom,597)
	NT38(x86_opcode_fcomp,598)
	NT37(x86_opcode_fcomp,599)
	NT36(x86_opcode_fcomp,600)
	NT35(x86_opcode_fcomp,601)
	NT34(x86_opcode_fcomp,602)
	NT33(x86_opcode_fcomp,603)
	NT32(x86_opcode_fcomp,604)
	NT31(x86_opcode_fcomp,605)
	NT38(x86_opcode_fsub,606)
	NT37(x86_opcode_fsub,607)
	NT36(x86_opcode_fsub,608)
	NT35(x86_opcode_fsub,609)
	NT34(x86_opcode_fsub,610)
	NT33(x86_opcode_fsub,611)
	NT32(x86_opcode_fsub,612)
	NT31(x86_opcode_fsub,613)
	NT38(x86_opcode_fsubr,614)
	NT37(x86_opcode_fsubr,615)
	NT36(x86_opcode_fsubr,616)
	NT35(x86_opcode_fsubr,617)
	NT34(x86_opcode_fsubr,618)
	NT33(x86_opcode_fsubr,619)
	NT32(x86_opcode_fsubr,620)
	NT31(x86_opcode_fsubr,621)
	NT38(x86_opcode_fdiv,622)
	NT37(x86_opcode_fdiv,623)
	NT36(x86_opcode_fdiv,624)
	NT35(x86_opcode_fdiv,625)
	NT34(x86_opcode_fdiv,626)
	NT33(x86_opcode_fdiv,627)
	NT32(x86_opcode_fdiv,628)
	NT31(x86_opcode_fdiv,629)
	NT38(x86_opcode_fdivr,630)
	NT37(x86_opcode_fdivr,631)
	NT36(x86_opcode_fdivr,632)
	NT35(x86_opcode_fdivr,633)
	NT34(x86_opcode_fdivr,634)
	NT33(x86_opcode_fdivr,635)
	NT32(x86_opcode_fdivr,636)
	NT31(x86_opcode_fdivr,637)
	NT59(x86_opcode_flds,638)
	NT59(x86_opcode_fsts,639)
	NT59(x86_opcode_fstps,640)
	NT59(x86_opcode_fldenv,641)
	NT59(x86_opcode_fldcw,642)
	NT59(x86_opcode_fstenv,643)
	NT59(x86_opcode_fstcw,644)
	NT38(x86_opcode_fld,645)
	NT37(x86_opcode_fld,646)
	NT36(x86_opcode_fld,647)
	NT35(x86_opcode_fld,648)
	NT34(x86_opcode_fld,649)
	NT33(x86_opcode_fld,650)
	NT32(x86_opcode_fld,651)
	NT31(x86_opcode_fld,652)
	NT38(x86_opcode_fxch,653)
	NT37(x86_opcode_fxch,654)
	NT36(x86_opcode_fxch,655)
	NT35(x86_opcode_fxch,656)
	NT34(x86_opcode_fxch,657)
	NT33(x86_opcode_fxch,658)
	NT32(x86_opcode_fxch,659)
	NT31(x86_opcode_fxch,660)
	NT0(x86_opcode_fnop,661)
	NT0(x86_opcode_fchs,662)
	NT0(x86_opcode_fabs,663)
	NT0(x86_opcode_ftst,664)
	NT0(x86_opcode_fxam,665)
	NT0(x86_opcode_fld1,666)
	NT0(x86_opcode_fldl2t,667)
	NT0(x86_opcode_fldl2e,668)
	NT0(x86_opcode_fldpi,669)
	NT0(x86_opcode_fldlg2,670)
	NT0(x86_opcode_fldln2,671)
	NT0(x86_opcode_fldz,672)
	NT0(x86_opcode_f2xm1,673)
	NT0(x86_opcode_fyl2x,674)
	NT0(x86_opcode_fptan,675)
	NT0(x86_opcode_fpatan,676)
	NT0(x86_opcode_fxtract,677)
	NT0(x86_opcode_fprem1,678)
	NT0(x86_opcode_fdecstp,679)
	NT0(x86_opcode_fincstp,680)
	NT0(x86_opcode_fprem,681)
	NT0(x86_opcode_fyl2xp1,682)
	NT0(x86_opcode_fsqrt,683)
	NT0(x86_opcode_fsincos,684)
	NT0(x86_opcode_frndint,685)
	NT0(x86_opcode_fscale,686)
	NT0(x86_opcode_fsin,687)
	NT0(x86_opcode_fcos,688)
	NT59(x86_opcode_fiaddl,689)
	NT59(x86_opcode_fimull,690)
	NT59(x86_opcode_ficoml,691)
	NT59(x86_opcode_ficompl,692)
	NT59(x86_opcode_fisubl,693)
	NT59(x86_opcode_fisubrl,694)
	NT59(x86_opcode_fidivl,695)
	NT59(x86_opcode_fidivrl,696)
	NT38(x86_opcode_fcmovb,697)
	NT37(x86_opcode_fcmovb,698)
	NT36(x86_opcode_fcmovb,699)
	NT35(x86_opcode_fcmovb,700)
	NT34(x86_opcode_fcmovb,701)
	NT33(x86_opcode_fcmovb,702)
	NT32(x86_opcode_fcmovb,703)
	NT31(x86_opcode_fcmovb,704)
	NT38(x86_opcode_fcmove,705)
	NT37(x86_opcode_fcmove,706)
	NT36(x86_opcode_fcmove,707)
	NT35(x86_opcode_fcmove,708)
	NT34(x86_opcode_fcmove,709)
	NT33(x86_opcode_fcmove,710)
	NT32(x86_opcode_fcmove,711)
	NT31(x86_opcode_fcmove,712)
	NT38(x86_opcode_fcmovbe,713)
	NT37(x86_opcode_fcmovbe,714)
	NT36(x86_opcode_fcmovbe,715)
	NT35(x86_opcode_fcmovbe,716)
	NT34(x86_opcode_fcmovbe,717)
	NT33(x86_opcode_fcmovbe,718)
	NT32(x86_opcode_fcmovbe,719)
	NT31(x86_opcode_fcmovbe,720)
	NT38(x86_opcode_fcmovu,721)
	NT37(x86_opcode_fcmovu,722)
	NT36(x86_opcode_fcmovu,723)
	NT35(x86_opcode_fcmovu,724)
	NT34(x86_opcode_fcmovu,725)
	NT33(x86_opcode_fcmovu,726)
	NT32(x86_opcode_fcmovu,727)
	NT31(x86_opcode_fcmovu,728)
	NT0(x86_opcode_fucompp,729)
	NT59(x86_opcode_fildl,730)
	NT59(x86_opcode_fistl,731)
	NT59(x86_opcode_fistpl,732)
	NT59(x86_opcode_fldt,733)
	NT59(x86_opcode_fstpt,734)
	NT38(x86_opcode_fcmovnb,735)
	NT37(x86_opcode_fcmovnb,736)
	NT36(x86_opcode_fcmovnb,737)
	NT35(x86_opcode_fcmovnb,738)
	NT34(x86_opcode_fcmovnb,739)
	NT33(x86_opcode_fcmovnb,740)
	NT32(x86_opcode_fcmovnb,741)
	NT31(x86_opcode_fcmovnb,742)
	NT38(x86_opcode_fcmovne,743)
	NT37(x86_opcode_fcmovne,744)
	NT36(x86_opcode_fcmovne,745)
	NT35(x86_opcode_fcmovne,746)
	NT34(x86_opcode_fcmovne,747)
	NT33(x86_opcode_fcmovne,748)
	NT32(x86_opcode_fcmovne,749)
	NT31(x86_opcode_fcmovne,750)
	NT38(x86_opcode_fcmovnbe,751)
	NT37(x86_opcode_fcmovnbe,752)
	NT36(x86_opcode_fcmovnbe,753)
	NT35(x86_opcode_fcmovnbe,754)
	NT34(x86_opcode_fcmovnbe,755)
	NT33(x86_opcode_fcmovnbe,756)
	NT32(x86_opcode_fcmovnbe,757)
	NT31(x86_opcode_fcmovnbe,758)
	NT38(x86_opcode_fcmovnu,759)
	NT37(x86_opcode_fcmovnu,760)
	NT36(x86_opcode_fcmovnu,761)
	NT35(x86_opcode_fcmovnu,762)
	NT34(x86_opcode_fcmovnu,763)
	NT33(x86_opcode_fcmovnu,764)
	NT32(x86_opcode_fcmovnu,765)
	NT31(x86_opcode_fcmovnu,766)
	NT0(x86_opcode_fclex,767)
	NT0(x86_opcode_finit,768)
	NT38(x86_opcode_fucomi,769)
	NT37(x86_opcode_fucomi,770)
	NT36(x86_opcode_fucomi,771)
	NT35(x86_opcode_fucomi,772)
	NT34(x86_opcode_fucomi,773)
	NT33(x86_opcode_fucomi,774)
	NT32(x86_opcode_fucomi,775)
	NT31(x86_opcode_fucomi,776)
	NT38(x86_opcode_fcomi,777)
	NT37(x86_opcode_fcomi,778)
	NT36(x86_opcode_fcomi,779)
	NT35(x86_opcode_fcomi,780)
	NT34(x86_opcode_fcomi,781)
	NT33(x86_opcode_fcomi,782)
	NT32(x86_opcode_fcomi,783)
	NT31(x86_opcode_fcomi,784)
	NT59(x86_opcode_faddl,785)
	NT59(x86_opcode_fmull,786)
	NT59(x86_opcode_fcoml,787)
	NT59(x86_opcode_fcompl,788)
	NT59(x86_opcode_fsubl,789)
	NT59(x86_opcode_fsubrl,790)
	NT59(x86_opcode_fdivl,791)
	NT59(x86_opcode_fdivrl,792)
	NT30(x86_opcode_fadd,794)
	NT29(x86_opcode_fadd,795)
	NT28(x86_opcode_fadd,796)
	NT27(x86_opcode_fadd,797)
	NT26(x86_opcode_fadd,798)
	NT25(x86_opcode_fadd,799)
	NT24(x86_opcode_fadd,800)
	NT30(x86_opcode_fmul,802)
	NT29(x86_opcode_fmul,803)
	NT28(x86_opcode_fmul,804)
	NT27(x86_opcode_fmul,805)
	NT26(x86_opcode_fmul,806)
	NT25(x86_opcode_fmul,807)
	NT24(x86_opcode_fmul,808)
	NT30(x86_opcode_fsubr,810)
	NT29(x86_opcode_fsubr,811)
	NT28(x86_opcode_fsubr,812)
	NT27(x86_opcode_fsubr,813)
	NT26(x86_opcode_fsubr,814)
	NT25(x86_opcode_fsubr,815)
	NT24(x86_opcode_fsubr,816)
	NT30(x86_opcode_fsub,818)
	NT29(x86_opcode_fsub,819)
	NT28(x86_opcode_fsub,820)
	NT27(x86_opcode_fsub,821)
	NT26(x86_opcode_fsub,822)
	NT25(x86_opcode_fsub,823)
	NT24(x86_opcode_fsub,824)
	NT30(x86_opcode_fdivr,826)
	NT29(x86_opcode_fdivr,827)
	NT28(x86_opcode_fdivr,828)
	NT27(x86_opcode_fdivr,829)
	NT26(x86_opcode_fdivr,830)
	NT25(x86_opcode_fdivr,831)
	NT24(x86_opcode_fdivr,832)
	NT30(x86_opcode_fdiv,834)
	NT29(x86_opcode_fdiv,835)
	NT28(x86_opcode_fdiv,836)
	NT27(x86_opcode_fdiv,837)
	NT26(x86_opcode_fdiv,838)
	NT25(x86_opcode_fdiv,839)
	NT24(x86_opcode_fdiv,840)
	NT59(x86_opcode_fldl,841)
	NT59(x86_opcode_fstl,842)
	NT59(x86_opcode_fstpl,843)
	NT59(x86_opcode_frstor,844)
	NT59(x86_opcode_fsave,845)
	NT59(x86_opcode_fstsw,846)
	NT23(x86_opcode_ffree,847)
	NT22(x86_opcode_ffree,848)
	NT21(x86_opcode_ffree,849)
	NT20(x86_opcode_ffree,850)
	NT19(x86_opcode_ffree,851)
	NT18(x86_opcode_ffree,852)
	NT17(x86_opcode_ffree,853)
	NT16(x86_opcode_ffree,854)
	NT23(x86_opcode_fst,855)
	NT22(x86_opcode_fst,856)
	NT21(x86_opcode_fst,857)
	NT20(x86_opcode_fst,858)
	NT19(x86_opcode_fst,859)
	NT18(x86_opcode_fst,860)
	NT17(x86_opcode_fst,861)
	NT16(x86_opcode_fst,862)
	NT23(x86_opcode_fstp,863)
	NT22(x86_opcode_fstp,864)
	NT21(x86_opcode_fstp,865)
	NT20(x86_opcode_fstp,866)
	NT19(x86_opcode_fstp,867)
	NT18(x86_opcode_fstp,868)
	NT17(x86_opcode_fstp,869)
	NT16(x86_opcode_fstp,870)
	NT38(x86_opcode_fucom,871)
	NT30(x86_opcode_fucom,872)
	NT29(x86_opcode_fucom,873)
	NT28(x86_opcode_fucom,874)
	NT27(x86_opcode_fucom,875)
	NT26(x86_opcode_fucom,876)
	NT25(x86_opcode_fucom,877)
	NT24(x86_opcode_fucom,878)
	NT23(x86_opcode_fucomp,879)
	NT22(x86_opcode_fucomp,880)
	NT21(x86_opcode_fucomp,881)
	NT20(x86_opcode_fucomp,882)
	NT19(x86_opcode_fucomp,883)
	NT18(x86_opcode_fucomp,884)
	NT17(x86_opcode_fucomp,885)
	NT16(x86_opcode_fucomp,886)
	NT59(x86_opcode_fiadds,887)
	NT59(x86_opcode_fimuls,888)
	NT59(x86_opcode_ficoms,889)
	NT59(x86_opcode_ficomps,890)
	NT59(x86_opcode_fisubs,891)
	NT59(x86_opcode_fisubrs,892)
	NT59(x86_opcode_fidivs,893)
	NT59(x86_opcode_fidivrs,894)
	NT38(x86_opcode_faddp,895)
	NT30(x86_opcode_faddp,896)
	NT29(x86_opcode_faddp,897)
	NT28(x86_opcode_faddp,898)
	NT27(x86_opcode_faddp,899)
	NT26(x86_opcode_faddp,900)
	NT25(x86_opcode_faddp,901)
	NT24(x86_opcode_faddp,902)
	NT38(x86_opcode_fmulp,903)
	NT30(x86_opcode_fmulp,904)
	NT29(x86_opcode_fmulp,905)
	NT28(x86_opcode_fmulp,906)
	NT27(x86_opcode_fmulp,907)
	NT26(x86_opcode_fmulp,908)
	NT25(x86_opcode_fmulp,909)
	NT24(x86_opcode_fmulp,910)
	NT0(x86_opcode_fcompp,911)
	NT38(x86_opcode_fsubrp,912)
	NT30(x86_opcode_fsubrp,913)
	NT29(x86_opcode_fsubrp,914)
	NT28(x86_opcode_fsubrp,915)
	NT27(x86_opcode_fsubrp,916)
	NT26(x86_opcode_fsubrp,917)
	NT25(x86_opcode_fsubrp,918)
	NT24(x86_opcode_fsubrp,919)
	NT38(x86_opcode_fsubp,920)
	NT30(x86_opcode_fsubp,921)
	NT29(x86_opcode_fsubp,922)
	NT28(x86_opcode_fsubp,923)
	NT27(x86_opcode_fsubp,924)
	NT26(x86_opcode_fsubp,925)
	NT25(x86_opcode_fsubp,926)
	NT24(x86_opcode_fsubp,927)
	NT38(x86_opcode_fdivrp,928)
	NT30(x86_opcode_fdivrp,929)
	NT29(x86_opcode_fdivrp,930)
	NT28(x86_opcode_fdivrp,931)
	NT27(x86_opcode_fdivrp,932)
	NT26(x86_opcode_fdivrp,933)
	NT25(x86_opcode_fdivrp,934)
	NT24(x86_opcode_fdivrp,935)
	NT38(x86_opcode_fdivp,936)
	NT30(x86_opcode_fdivp,937)
	NT29(x86_opcode_fdivp,938)
	NT28(x86_opcode_fdivp,939)
	NT27(x86_opcode_fdivp,940)
	NT26(x86_opcode_fdivp,941)
	NT25(x86_opcode_fdivp,942)
	NT24(x86_opcode_fdivp,943)
	NT59(x86_opcode_filds,944)
	NT59(x86_opcode_fists,945)
	NT59(x86_opcode_fistps,946)
	NT59(x86_opcode_fbldt,947)
	NT59(x86_opcode_fildq,948)
	NT59(x86_opcode_fbstp,949)
	NT59(x86_opcode_fistpq,950)
	NT15(x86_opcode_fstsw,951)
	NT38(x86_opcode_fucomip,952)
	NT37(x86_opcode_fucomip,953)
	NT36(x86_opcode_fucomip,954)
	NT35(x86_opcode_fucomip,955)
	NT34(x86_opcode_fucomip,956)
	NT33(x86_opcode_fucomip,957)
	NT32(x86_opcode_fucomip,958)
	NT31(x86_opcode_fucomip,959)
	NT38(x86_opcode_fcomip,960)
	NT37(x86_opcode_fcomip,961)
	NT36(x86_opcode_fcomip,962)
	NT35(x86_opcode_fcomip,963)
	NT34(x86_opcode_fcomip,964)
	NT33(x86_opcode_fcomip,965)
	NT32(x86_opcode_fcomip,966)
	NT31(x86_opcode_fcomip,967)
	NT14(x86_opcode_addb,968)
	NT14(x86_opcode_orb,969)
	NT14(x86_opcode_adcb,970)
	NT14(x86_opcode_sbbb,971)
	NT14(x86_opcode_andb,972)
	NT14(x86_opcode_subb,973)
	NT14(x86_opcode_xorb,974)
	NT14(x86_opcode_cmpb,975)
	NT13(x86_opcode_addl,976)
	NT13(x86_opcode_orl,977)
	NT13(x86_opcode_adcl,978)
	NT13(x86_opcode_sbbl,979)
	NT13(x86_opcode_andl,980)
	NT13(x86_opcode_subl,981)
	NT13(x86_opcode_xorl,982)
	NT13(x86_opcode_cmpl,983)
	NT12(x86_opcode_add,984)
	NT12(x86_opcode_or,985)
	NT12(x86_opcode_adc,986)
	NT12(x86_opcode_sbb,987)
	NT12(x86_opcode_and,988)
	NT12(x86_opcode_sub,989)
	NT12(x86_opcode_xor,990)
	NT12(x86_opcode_cmp,991)
	NT11(x86_opcode_rolb,992)
	NT11(x86_opcode_rorb,993)
	NT11(x86_opcode_rclb,994)
	NT11(x86_opcode_rcrb,995)
	NT11(x86_opcode_shlb,996)
	NT11(x86_opcode_shrb,997)
	NT11(x86_opcode_sarb,998)
	NT12(x86_opcode_rol,999)
	NT12(x86_opcode_ror,1000)
	NT12(x86_opcode_rcl,1001)
	NT12(x86_opcode_rcr,1002)
	NT12(x86_opcode_shl,1003)
	NT12(x86_opcode_shr,1004)
	NT12(x86_opcode_sar,1005)
	NT10(x86_opcode_rolb,1006)
	NT10(x86_opcode_rorb,1007)
	NT10(x86_opcode_rclb,1008)
	NT10(x86_opcode_rcrb,1009)
	NT10(x86_opcode_shlb,1010)
	NT10(x86_opcode_shrb,1011)
	NT10(x86_opcode_sarb,1012)
	NT9(x86_opcode_rol,1013)
	NT9(x86_opcode_ror,1014)
	NT9(x86_opcode_rcl,1015)
	NT9(x86_opcode_rcr,1016)
	NT9(x86_opcode_shl,1017)
	NT9(x86_opcode_shr,1018)
	NT9(x86_opcode_sar,1019)
	NT8(x86_opcode_rolb,1020)
	NT8(x86_opcode_rorb,1021)
	NT8(x86_opcode_rclb,1022)
	NT8(x86_opcode_rcrb,1023)
	NT8(x86_opcode_shlb,1024)
	NT8(x86_opcode_shrb,1025)
	NT8(x86_opcode_sarb,1026)
	NT7(x86_opcode_rol,1027)
	NT7(x86_opcode_ror,1028)
	NT7(x86_opcode_rcl,1029)
	NT7(x86_opcode_rcr,1030)
	NT7(x86_opcode_shl,1031)
	NT7(x86_opcode_shr,1032)
	NT7(x86_opcode_sar,1033)
	NT14(x86_opcode_testb,1034)
	NT64(x86_opcode_notb,1035)
	NT64(x86_opcode_negb,1036)
	NT64(x86_opcode_mulb,1037)
	NT64(x86_opcode_imulb,1038)
	NT64(x86_opcode_divb,1039)
	NT64(x86_opcode_idivb,1040)
	NT13(x86_opcode_testl,1041)
	NT6(x86_opcode_notl,1042)
	NT6(x86_opcode_negl,1043)
	NT6(x86_opcode_mull,1044)
	NT6(x86_opcode_imull,1045)
	NT6(x86_opcode_divl,1046)
	NT6(x86_opcode_idivl,1047)
	NT64(x86_opcode_incb,1048)
	NT64(x86_opcode_decb,1049)
	NT129(x86_opcode_incl,1050)
	NT129(x86_opcode_decl,1051)
	NT6(x86_opcode_calll,1052)
	NT6(x86_opcode_lcalll,1053)
	NT6(x86_opcode_jmpl,1054)
	NT6(x86_opcode_ljmpl,1055)
	NT129(x86_opcode_pushl,1056)
	NT5(x86_opcode_sldtw,1057)
	NT6(x86_opcode_strl,1058)
	NT5(x86_opcode_lldtw,1059)
	NT5(x86_opcode_ltrw,1060)
	NT5(x86_opcode_verrw,1061)
	NT5(x86_opcode_verww,1062)
	NT59(x86_opcode_sgdtl,1063)
	NT59(x86_opcode_sidtl,1064)
	NT59(x86_opcode_lgdtl,1065)
	NT59(x86_opcode_lidtl,1066)
	NT5(x86_opcode_smsww,1067)
	NT5(x86_opcode_lmsww,1068)
	NT59(x86_opcode_invlpgb,1069)
	NT12(x86_opcode_bt,1070)
	NT12(x86_opcode_bts,1071)
	NT12(x86_opcode_btr,1072)
	NT12(x86_opcode_btc,1073)
	NT59(x86_opcode_cmpxch8bl,1074)
	NT4(x86_opcode_movb,1075)
	NT3(x86_opcode_movl,1076)
	NT2(x86_opcode_psrlw,1077)
	NT2(x86_opcode_psraw,1078)
	NT2(x86_opcode_psllw,1079)
	NT2(x86_opcode_psrld,1083)
	NT2(x86_opcode_psrad,1084)
	NT2(x86_opcode_pslld,1085)
	NT1(x86_opcode_psrld,1086)
	NT1(x86_opcode_psrad,1087)
	NT1(x86_opcode_pslld,1088)
	NT2(x86_opcode_psrlq,1089)
	NT2(x86_opcode_psllq,1090)
	NT1(x86_opcode_psrlq,1091)
	NT1(x86_opcode_psrldq,1092)
	NT1(x86_opcode_psllq,1093)
	NT1(x86_opcode_pslldq,1094)
	NT0(x86_opcode_fxsave,1095)
	NT0(x86_opcode_fxrstor,1096)
	NT0(x86_opcode_ldmxcsr,1097)
	NT0(x86_opcode_stmxcsr,1098)
	NT0(x86_opcode_lfence,1099)
	NT0(x86_opcode_mfence,1100)
	NT0(x86_opcode_sfence,1101)
	NT0(x86_opcode_clflush,1102)
	NT0(x86_opcode_prefetchnta,1103)
	NT0(x86_opcode_prefetcht0,1104)
	NT0(x86_opcode_prefetcht1,1105)
	NT0(x86_opcode_prefetcht2,1106)
};

#ifdef AVS_X86_OPCODE_TABLE
static X86OpcodeTable opcodetable[] = {
	/*    0 */ { "add"       , 1, { 0x00, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*    1 */ { "add"       , 1, { 0x01, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*    2 */ { "add"       , 1, { 0x02, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*    3 */ { "add"       , 1, { 0x03, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*    4 */ { "add"       , 1, { 0x04, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*    5 */ { "add"       , 1, { 0x05, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*    6 */ { "push"      , 1, { 0x06, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(es)),
			},},
	/*    7 */ { "pop"       , 1, { 0x07, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(es)),
			},},
	/*    8 */ { "or"        , 1, { 0x08, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*    9 */ { "or"        , 1, { 0x09, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   10 */ { "or"        , 1, { 0x0a, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   11 */ { "or"        , 1, { 0x0b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   12 */ { "or"        , 1, { 0x0c, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   13 */ { "or"        , 1, { 0x0d, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   14 */ { "push"      , 1, { 0x0e, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(cs)),
			},},
	/*   15 */ { "adc"       , 1, { 0x10, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   16 */ { "adc"       , 1, { 0x11, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   17 */ { "adc"       , 1, { 0x12, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   18 */ { "adc"       , 1, { 0x13, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   19 */ { "adc"       , 1, { 0x14, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   20 */ { "adc"       , 1, { 0x15, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   21 */ { "push"      , 1, { 0x16, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ss)),
			},},
	/*   22 */ { "pop"       , 1, { 0x17, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ss)),
			},},
	/*   23 */ { "sbb"       , 1, { 0x18, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   24 */ { "sbb"       , 1, { 0x19, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   25 */ { "sbb"       , 1, { 0x1a, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   26 */ { "sbb"       , 1, { 0x1b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   27 */ { "sbb"       , 1, { 0x1c, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   28 */ { "sbb"       , 1, { 0x1d, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   29 */ { "push"      , 1, { 0x1e, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ds)),
			},},
	/*   30 */ { "pop"       , 1, { 0x1f, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ds)),
			},},
	/*   31 */ { "and"       , 1, { 0x20, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   32 */ { "and"       , 1, { 0x21, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   33 */ { "and"       , 1, { 0x22, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   34 */ { "and"       , 1, { 0x23, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   35 */ { "and"       , 1, { 0x24, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   36 */ { "and"       , 1, { 0x25, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   37 */ { "daa"       , 1, { 0x27, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   38 */ { "sub"       , 1, { 0x28, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   39 */ { "sub"       , 1, { 0x29, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   40 */ { "sub"       , 1, { 0x2a, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   41 */ { "sub"       , 1, { 0x2b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   42 */ { "sub"       , 1, { 0x2c, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   43 */ { "sub"       , 1, { 0x2d, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   44 */ { "das"       , 1, { 0x2f, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   45 */ { "xor"       , 1, { 0x30, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   46 */ { "xor"       , 1, { 0x31, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   47 */ { "xor"       , 1, { 0x32, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   48 */ { "xor"       , 1, { 0x33, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   49 */ { "xor"       , 1, { 0x34, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   50 */ { "xor"       , 1, { 0x35, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   51 */ { "aaa"       , 1, { 0x37, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   52 */ { "cmp"       , 1, { 0x38, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*   53 */ { "cmp"       , 1, { 0x39, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*   54 */ { "cmp"       , 1, { 0x3a, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*   55 */ { "cmp"       , 1, { 0x3b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*   56 */ { "cmp"       , 1, { 0x3c, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*   57 */ { "cmp"       , 1, { 0x3d, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*   58 */ { "aas"       , 1, { 0x3f, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   59 */ { "inc"       , 1, { 0x40, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*   60 */ { "inc"       , 1, { 0x41, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*   61 */ { "inc"       , 1, { 0x42, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*   62 */ { "inc"       , 1, { 0x43, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*   63 */ { "inc"       , 1, { 0x44, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*   64 */ { "inc"       , 1, { 0x45, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*   65 */ { "inc"       , 1, { 0x46, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*   66 */ { "inc"       , 1, { 0x47, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*   67 */ { "dec"       , 1, { 0x48, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*   68 */ { "dec"       , 1, { 0x49, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*   69 */ { "dec"       , 1, { 0x4a, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*   70 */ { "dec"       , 1, { 0x4b, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*   71 */ { "dec"       , 1, { 0x4c, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*   72 */ { "dec"       , 1, { 0x4d, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*   73 */ { "dec"       , 1, { 0x4e, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*   74 */ { "dec"       , 1, { 0x4f, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*   75 */ { "push"      , 1, { 0x50, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*   76 */ { "push"      , 1, { 0x51, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*   77 */ { "push"      , 1, { 0x52, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*   78 */ { "push"      , 1, { 0x53, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*   79 */ { "push"      , 1, { 0x54, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*   80 */ { "push"      , 1, { 0x55, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*   81 */ { "push"      , 1, { 0x56, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*   82 */ { "push"      , 1, { 0x57, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*   83 */ { "pop"       , 1, { 0x58, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*   84 */ { "pop"       , 1, { 0x59, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*   85 */ { "pop"       , 1, { 0x5a, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*   86 */ { "pop"       , 1, { 0x5b, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*   87 */ { "pop"       , 1, { 0x5c, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*   88 */ { "pop"       , 1, { 0x5d, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*   89 */ { "pop"       , 1, { 0x5e, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*   90 */ { "pop"       , 1, { 0x5f, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*   91 */ { "pusha"     , 1, { 0x60, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   92 */ { "popa"      , 1, { 0x61, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   93 */ { "bound"     , 1, { 0x62, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 8),
			},},
	/*   94 */ { "arpl"      , 1, { 0x63, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_word, 2),
				X86_OP(2, register_word, 2),
			},},
	/*   95 */ { "opdsize"   , 1, { 0x66, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   96 */ { "addrsize"  , 1, { 0x67, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*   97 */ { "push"      , 1, { 0x68, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 4),
			},},
	/*   98 */ { "imul"      , 1, { 0x69, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*   99 */ { "push"      , 1, { 0x6a, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 1),
			},},
	/*  100 */ { "imul"      , 1, { 0x6b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  101 */ { "insb"      , 1, { 0x6c, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 1),
				X86_OP(0, register, X86_REGISTER(dx)),
			},},
	/*  102 */ { "insd"      , 1, { 0x6d, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 4),
				X86_OP(0, register, X86_REGISTER(dx)),
			},},
	/*  103 */ { "outsb"     , 1, { 0x6e, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dx)),
				X86_OP(0, ignored, 1),
			},},
	/*  104 */ { "outsd"     , 1, { 0x6f, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dx)),
				X86_OP(0, ignored, 4),
			},},
	/*  105 */ { "jo"        , 1, { 0x70, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  106 */ { "jno"       , 1, { 0x71, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  107 */ { "jb"        , 1, { 0x72, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  108 */ { "jnb"       , 1, { 0x73, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  109 */ { "jz"        , 1, { 0x74, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  110 */ { "jnz"       , 1, { 0x75, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  111 */ { "jbe"       , 1, { 0x76, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  112 */ { "jnbe"      , 1, { 0x77, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  113 */ { "js"        , 1, { 0x78, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  114 */ { "jns"       , 1, { 0x79, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  115 */ { "jp"        , 1, { 0x7a, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  116 */ { "jnp"       , 1, { 0x7b, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  117 */ { "jl"        , 1, { 0x7c, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  118 */ { "jnl"       , 1, { 0x7d, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  119 */ { "jle"       , 1, { 0x7e, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  120 */ { "jnle"      , 1, { 0x7f, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  121 */ { "test"      , 1, { 0x84, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*  122 */ { "test"      , 1, { 0x85, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  123 */ { "xchg"      , 1, { 0x86, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*  124 */ { "xchg"      , 1, { 0x87, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  125 */ { "mov"       , 1, { 0x88, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*  126 */ { "mov"       , 1, { 0x89, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  127 */ { "mov"       , 1, { 0x8a, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_byte, 1),
				X86_OP(1, register_byte, 1),
			},},
	/*  128 */ { "mov"       , 1, { 0x8b, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  129 */ { "mov"       , 1, { 0x8c, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_word, 2),
				X86_OP(2, register_segment, 2),
			},},
	/*  130 */ { "lea"       , 1, { 0x8d, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 8),
			},},
	/*  131 */ { "mov"       , 1, { 0x8e, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_segment, 2),
				X86_OP(1, register_word, 2),
			},},
	/*  132 */ { "pop"       , 1, { 0x8f, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  133 */ { "nop"       , 1, { 0x90, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  134 */ { "xchg"      , 1, { 0x91, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*  135 */ { "xchg"      , 1, { 0x92, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*  136 */ { "xchg"      , 1, { 0x93, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*  137 */ { "xchg"      , 1, { 0x94, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*  138 */ { "xchg"      , 1, { 0x95, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*  139 */ { "xchg"      , 1, { 0x96, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*  140 */ { "xchg"      , 1, { 0x97, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*  141 */ { "cbw"       , 1, { 0x98, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  142 */ { "cwd"       , 1, { 0x99, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  143 */ { "lcall"     , 1, { 0x9a, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, offset, 4),
				X86_OP(0, immediate, 2),
			},},
	/*  144 */ { "fwait"     , 1, { 0x9b, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  145 */ { "pushf"     , 1, { 0x9c, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, ignored, 4),
			},},
	/*  146 */ { "popf"      , 1, { 0x9d, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, ignored, 4),
			},},
	/*  147 */ { "sahf"      , 1, { 0x9e, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  148 */ { "lahf"      , 1, { 0x9f, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  149 */ { "mov"       , 1, { 0xa0, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, offset, 1),
			},},
	/*  150 */ { "mov"       , 1, { 0xa1, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, offset, 4),
			},},
	/*  151 */ { "mov"       , 1, { 0xa2, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, offset, 1),
				X86_OP(0, register, X86_REGISTER(al)),
			},},
	/*  152 */ { "mov"       , 1, { 0xa3, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, offset, 4),
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*  153 */ { "movsb"     , 1, { 0xa4, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 1),
				X86_OP(0, ignored, 1),
			},},
	/*  154 */ { "movsd"     , 1, { 0xa5, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 4),
				X86_OP(0, ignored, 4),
			},},
	/*  155 */ { "cmpsb"     , 1, { 0xa6, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 1),
				X86_OP(0, ignored, 1),
			},},
	/*  156 */ { "cmpsd"     , 1, { 0xa7, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 4),
				X86_OP(0, ignored, 4),
			},},
	/*  157 */ { "test"      , 1, { 0xa8, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*  158 */ { "test"      , 1, { 0xa9, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*  159 */ { "stosb"     , 1, { 0xaa, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 1),
				X86_OP(0, register, X86_REGISTER(al)),
			},},
	/*  160 */ { "stosd"     , 1, { 0xab, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, ignored, 4),
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*  161 */ { "lodsb"     , 1, { 0xac, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, ignored, 1),
			},},
	/*  162 */ { "lodsd"     , 1, { 0xad, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, ignored, 4),
			},},
	/*  163 */ { "scasb"     , 1, { 0xae, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, ignored, 1),
			},},
	/*  164 */ { "scasd"     , 1, { 0xaf, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, ignored, 4),
			},},
	/*  165 */ { "mov"       , 1, { 0xb0, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*  166 */ { "mov"       , 1, { 0xb1, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(cl)),
				X86_OP(0, immediate, 1),
			},},
	/*  167 */ { "mov"       , 1, { 0xb2, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dl)),
				X86_OP(0, immediate, 1),
			},},
	/*  168 */ { "mov"       , 1, { 0xb3, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(bl)),
				X86_OP(0, immediate, 1),
			},},
	/*  169 */ { "mov"       , 1, { 0xb4, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(ah)),
				X86_OP(0, immediate, 1),
			},},
	/*  170 */ { "mov"       , 1, { 0xb5, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(ch)),
				X86_OP(0, immediate, 1),
			},},
	/*  171 */ { "mov"       , 1, { 0xb6, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dh)),
				X86_OP(0, immediate, 1),
			},},
	/*  172 */ { "mov"       , 1, { 0xb7, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(bh)),
				X86_OP(0, immediate, 1),
			},},
	/*  173 */ { "mov"       , 1, { 0xb8, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 4),
			},},
	/*  174 */ { "mov"       , 1, { 0xb9, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(ecx)),
				X86_OP(0, immediate, 4),
			},},
	/*  175 */ { "mov"       , 1, { 0xba, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(edx)),
				X86_OP(0, immediate, 4),
			},},
	/*  176 */ { "mov"       , 1, { 0xbb, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(ebx)),
				X86_OP(0, immediate, 4),
			},},
	/*  177 */ { "mov"       , 1, { 0xbc, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(esp)),
				X86_OP(0, immediate, 4),
			},},
	/*  178 */ { "mov"       , 1, { 0xbd, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(ebp)),
				X86_OP(0, immediate, 4),
			},},
	/*  179 */ { "mov"       , 1, { 0xbe, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(esi)),
				X86_OP(0, immediate, 4),
			},},
	/*  180 */ { "mov"       , 1, { 0xbf, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(edi)),
				X86_OP(0, immediate, 4),
			},},
	/*  181 */ { "ret"       , 1, { 0xc2, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 2),
			},},
	/*  182 */ { "ret"       , 1, { 0xc3, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  183 */ { "les"       , 1, { 0xc4, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 6),
			},},
	/*  184 */ { "lds"       , 1, { 0xc5, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 6),
			},},
	/*  185 */ { "enter"     , 1, { 0xc8, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, immediate, 2),
				X86_OP(0, immediate, 1),
			},},
	/*  186 */ { "leave"     , 1, { 0xc9, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  187 */ { "retf"      , 1, { 0xca, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 2),
			},},
	/*  188 */ { "retf"      , 1, { 0xcb, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  189 */ { "int3"      , 1, { 0xcc, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  190 */ { "int"       , 1, { 0xcd, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 1),
			},},
	/*  191 */ { "into"      , 1, { 0xce, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  192 */ { "iret"      , 1, { 0xcf, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  193 */ { "aam"       , 1, { 0xd4, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 1),
			},},
	/*  194 */ { "aad"       , 1, { 0xd5, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, immediate, 1),
			},},
	/*  195 */ { "xlat"      , 1, { 0xd7, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  196 */ { "loopne"    , 1, { 0xe0, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  197 */ { "loope"     , 1, { 0xe1, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  198 */ { "loop"      , 1, { 0xe2, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  199 */ { "jcxz"      , 1, { 0xe3, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  200 */ { "in"        , 1, { 0xe4, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, immediate, 1),
			},},
	/*  201 */ { "in"        , 1, { 0xe5, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, immediate, 1),
			},},
	/*  202 */ { "out"       , 1, { 0xe6, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, immediate, 1),
				X86_OP(0, register, X86_REGISTER(al)),
			},},
	/*  203 */ { "out"       , 1, { 0xe7, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, immediate, 1),
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*  204 */ { "call"      , 1, { 0xe8, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  205 */ { "jmp"       , 1, { 0xe9, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  206 */ { "ljmp"      , 1, { 0xea, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, offset, 4),
				X86_OP(0, immediate, 2),
			},},
	/*  207 */ { "jmp"       , 1, { 0xeb, 0x00, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 1),
			},},
	/*  208 */ { "in"        , 1, { 0xec, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(al)),
				X86_OP(0, register, X86_REGISTER(dx)),
			},},
	/*  209 */ { "in"        , 1, { 0xed, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(eax)),
				X86_OP(0, register, X86_REGISTER(dx)),
			},},
	/*  210 */ { "out"       , 1, { 0xee, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dx)),
				X86_OP(0, register, X86_REGISTER(al)),
			},},
	/*  211 */ { "out"       , 1, { 0xef, 0x00, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(dx)),
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*  212 */ { "lock"      , 1, { 0xf0, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  213 */ { "repne"     , 1, { 0xf2, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  214 */ { "rep"       , 1, { 0xf3, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  215 */ { "hlt"       , 1, { 0xf4, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  216 */ { "cmc"       , 1, { 0xf5, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  217 */ { "clc"       , 1, { 0xf8, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  218 */ { "stc"       , 1, { 0xf9, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  219 */ { "cli"       , 1, { 0xfa, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  220 */ { "sti"       , 1, { 0xfb, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  221 */ { "cld"       , 1, { 0xfc, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  222 */ { "std"       , 1, { 0xfd, 0x00, 0x00, 0x00 }, 0, 0, {}, },
	/*  223 */ { "lar"       , 2, { 0x0f, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_word, 2),
			},},
	/*  224 */ { "lsl"       , 2, { 0x0f, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_word, 2),
			},},
	/*  225 */ { "clts"      , 2, { 0x0f, 0x06, 0x00, 0x00 }, 0, 0, {}, },
	/*  226 */ { "invd"      , 2, { 0x0f, 0x08, 0x00, 0x00 }, 0, 0, {}, },
	/*  227 */ { "wbinvd"    , 2, { 0x0f, 0x09, 0x00, 0x00 }, 0, 0, {}, },
	/*  228 */ { "ud2a"      , 2, { 0x0f, 0x0b, 0x00, 0x00 }, 0, 0, {}, },
	/*  229 */ { "movups"    , 2, { 0x0f, 0x10, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  230 */ { "movups"    , 2, { 0x0f, 0x11, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  231 */ { "movlps"    , 2, { 0x0f, 0x12, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 8),
				X86_OP(2, register_xmm, 8),
			},},
	/*  232 */ { "movlps"    , 2, { 0x0f, 0x13, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  233 */ { "unpcklps"  , 2, { 0x0f, 0x14, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 8),
			},},
	/*  234 */ { "unpckhps"  , 2, { 0x0f, 0x15, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 8),
			},},
	/*  235 */ { "movhps"    , 2, { 0x0f, 0x16, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  236 */ { "movhps"    , 2, { 0x0f, 0x17, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 8),
				X86_OP(2, register_xmm, 8),
			},},
	/*  237 */ { "mov"       , 2, { 0x0f, 0x20, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_control, 4),
			},},
	/*  238 */ { "mov"       , 2, { 0x0f, 0x21, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_debug, 4),
			},},
	/*  239 */ { "mov"       , 2, { 0x0f, 0x22, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_control, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  240 */ { "mov"       , 2, { 0x0f, 0x23, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_debug, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  241 */ { "movaps"    , 2, { 0x0f, 0x28, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  242 */ { "movaps"    , 2, { 0x0f, 0x29, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  243 */ { "cvtpi2ps"  , 2, { 0x0f, 0x2a, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_mmx, 8),
			},},
	/*  244 */ { "movntps"   , 2, { 0x0f, 0x2b, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  245 */ { "cvttps2pi" , 2, { 0x0f, 0x2c, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_mmx, 8),
				X86_OP(1, register_xmm, 16),
			},},
	/*  246 */ { "cvtps2pi"  , 2, { 0x0f, 0x2d, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_mmx, 8),
				X86_OP(1, register_xmm, 16),
			},},
	/*  247 */ { "ucomiss"   , 2, { 0x0f, 0x2e, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  248 */ { "comiss"    , 2, { 0x0f, 0x2f, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  249 */ { "wrmsr"     , 2, { 0x0f, 0x30, 0x00, 0x00 }, 0, 0, {}, },
	/*  250 */ { "rdtsc"     , 2, { 0x0f, 0x31, 0x00, 0x00 }, 0, 0, {}, },
	/*  251 */ { "rdmsr"     , 2, { 0x0f, 0x32, 0x00, 0x00 }, 0, 0, {}, },
	/*  252 */ { "rdpmc"     , 2, { 0x0f, 0x33, 0x00, 0x00 }, 0, 0, {}, },
	/*  253 */ { "sysenter"  , 2, { 0x0f, 0x34, 0x00, 0x00 }, 0, 0, {}, },
	/*  254 */ { "sysexit"   , 2, { 0x0f, 0x35, 0x00, 0x00 }, 0, 0, {}, },
	/*  255 */ { "movnti"    , 2, { 0x0f, 0x3c, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  256 */ { "cmovo"     , 2, { 0x0f, 0x40, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  257 */ { "cmovno"    , 2, { 0x0f, 0x41, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  258 */ { "cmovb"     , 2, { 0x0f, 0x42, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  259 */ { "cmovae"    , 2, { 0x0f, 0x43, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  260 */ { "cmove"     , 2, { 0x0f, 0x44, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  261 */ { "cmovne"    , 2, { 0x0f, 0x45, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  262 */ { "cmovbe"    , 2, { 0x0f, 0x46, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  263 */ { "cmova"     , 2, { 0x0f, 0x47, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  264 */ { "cmovs"     , 2, { 0x0f, 0x48, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  265 */ { "cmovns"    , 2, { 0x0f, 0x49, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  266 */ { "cmovp"     , 2, { 0x0f, 0x4a, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  267 */ { "cmovnp"    , 2, { 0x0f, 0x4b, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  268 */ { "cmovl"     , 2, { 0x0f, 0x4c, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  269 */ { "cmovnl"    , 2, { 0x0f, 0x4d, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  270 */ { "cmovle"    , 2, { 0x0f, 0x4e, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  271 */ { "cmovnle"   , 2, { 0x0f, 0x4f, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  272 */ { "movmskps"  , 2, { 0x0f, 0x50, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_xmm, 16),
			},},
	/*  273 */ { "sqrtps"    , 2, { 0x0f, 0x51, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  274 */ { "rsqrtps"   , 2, { 0x0f, 0x52, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  275 */ { "rcpps"     , 2, { 0x0f, 0x53, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  276 */ { "andps"     , 2, { 0x0f, 0x54, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  277 */ { "andnps"    , 2, { 0x0f, 0x55, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  278 */ { "orps"      , 2, { 0x0f, 0x56, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  279 */ { "xorps"     , 2, { 0x0f, 0x57, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  280 */ { "addps"     , 2, { 0x0f, 0x58, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  281 */ { "mulps"     , 2, { 0x0f, 0x59, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  282 */ { "cvtps2pd"  , 2, { 0x0f, 0x5a, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  283 */ { "cvtdq2ps"  , 2, { 0x0f, 0x5b, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  284 */ { "subps"     , 2, { 0x0f, 0x5c, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  285 */ { "minps"     , 2, { 0x0f, 0x5d, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  286 */ { "divps"     , 2, { 0x0f, 0x5e, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  287 */ { "maxps"     , 2, { 0x0f, 0x5f, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  288 */ { "punpcklbw" , 2, { 0x0f, 0x60, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  289 */ { "punpcklwd" , 2, { 0x0f, 0x61, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  290 */ { "punpckldq" , 2, { 0x0f, 0x62, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  291 */ { "packsswb"  , 2, { 0x0f, 0x63, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  292 */ { "pcmpgtb"   , 2, { 0x0f, 0x64, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  293 */ { "pcmpgtw"   , 2, { 0x0f, 0x65, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  294 */ { "pcmpgtd"   , 2, { 0x0f, 0x66, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  295 */ { "packuswb"  , 2, { 0x0f, 0x67, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  296 */ { "punpckhbw" , 2, { 0x0f, 0x68, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  297 */ { "punpckhwd" , 2, { 0x0f, 0x69, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  298 */ { "punpckhdq" , 2, { 0x0f, 0x6a, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  299 */ { "packssdw"  , 2, { 0x0f, 0x6b, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 4),
			},},
	/*  300 */ { "movd"      , 2, { 0x0f, 0x6e, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  301 */ { "movq"      , 2, { 0x0f, 0x6f, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  302 */ { "pshufw"    , 2, { 0x0f, 0x70, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/*  303 */ { "pcmpeqb"   , 2, { 0x0f, 0x74, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  304 */ { "pcmpeqw"   , 2, { 0x0f, 0x75, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  305 */ { "pcmpeqd"   , 2, { 0x0f, 0x76, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  306 */ { "emms"      , 2, { 0x0f, 0x77, 0x00, 0x00 }, 0, 0, {}, },
	/*  307 */ { "movd"      , 2, { 0x0f, 0x7e, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_mmx, 4),
			},},
	/*  308 */ { "movq"      , 2, { 0x0f, 0x7f, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_mmx, 8),
				X86_OP(2, register_mmx, 8),
			},},
	/*  309 */ { "jo"        , 2, { 0x0f, 0x80, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  310 */ { "jno"       , 2, { 0x0f, 0x81, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  311 */ { "jb"        , 2, { 0x0f, 0x82, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  312 */ { "jae"       , 2, { 0x0f, 0x83, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  313 */ { "je"        , 2, { 0x0f, 0x84, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  314 */ { "jne"       , 2, { 0x0f, 0x85, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  315 */ { "jbe"       , 2, { 0x0f, 0x86, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  316 */ { "ja"        , 2, { 0x0f, 0x87, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  317 */ { "js"        , 2, { 0x0f, 0x88, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  318 */ { "jns"       , 2, { 0x0f, 0x89, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  319 */ { "jp"        , 2, { 0x0f, 0x8a, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  320 */ { "jnp"       , 2, { 0x0f, 0x8b, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  321 */ { "jl"        , 2, { 0x0f, 0x8c, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  322 */ { "jnl"       , 2, { 0x0f, 0x8d, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  323 */ { "jle"       , 2, { 0x0f, 0x8e, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  324 */ { "jnle"      , 2, { 0x0f, 0x8f, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, relative, 4),
			},},
	/*  325 */ { "seto"      , 2, { 0x0f, 0x90, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  326 */ { "setno"     , 2, { 0x0f, 0x91, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  327 */ { "setb"      , 2, { 0x0f, 0x92, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  328 */ { "setae"     , 2, { 0x0f, 0x93, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  329 */ { "sete"      , 2, { 0x0f, 0x94, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  330 */ { "setne"     , 2, { 0x0f, 0x95, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  331 */ { "setbe"     , 2, { 0x0f, 0x96, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  332 */ { "seta"      , 2, { 0x0f, 0x97, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  333 */ { "sets"      , 2, { 0x0f, 0x98, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  334 */ { "setns"     , 2, { 0x0f, 0x99, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  335 */ { "setp"      , 2, { 0x0f, 0x9a, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  336 */ { "setnp"     , 2, { 0x0f, 0x9b, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  337 */ { "setl"      , 2, { 0x0f, 0x9c, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  338 */ { "setnl"     , 2, { 0x0f, 0x9d, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  339 */ { "setle"     , 2, { 0x0f, 0x9e, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  340 */ { "setnle"    , 2, { 0x0f, 0x9f, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/*  341 */ { "push"      , 2, { 0x0f, 0xa0, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(fs)),
			},},
	/*  342 */ { "pop"       , 2, { 0x0f, 0xa1, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(fs)),
			},},
	/*  343 */ { "cpuid"     , 2, { 0x0f, 0xa2, 0x00, 0x00 }, 0, 0, {}, },
	/*  344 */ { "bt"        , 2, { 0x0f, 0xa3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  345 */ { "shld"      , 2, { 0x0f, 0xa4, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  346 */ { "shld"      , 2, { 0x0f, 0xa5, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/*  347 */ { "push"      , 2, { 0x0f, 0xa8, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(gs)),
			},},
	/*  348 */ { "pop"       , 2, { 0x0f, 0xa9, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(gs)),
			},},
	/*  349 */ { "rsm"       , 2, { 0x0f, 0xaa, 0x00, 0x00 }, 0, 0, {}, },
	/*  350 */ { "bts"       , 2, { 0x0f, 0xab, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  351 */ { "shrd"      , 2, { 0x0f, 0xac, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  352 */ { "shrd"      , 2, { 0x0f, 0xad, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/*  353 */ { "imul"      , 2, { 0x0f, 0xaf, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  354 */ { "cmpxchg"   , 2, { 0x0f, 0xb0, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*  355 */ { "cmpxchg"   , 2, { 0x0f, 0xb1, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  356 */ { "lss"       , 2, { 0x0f, 0xb2, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/*  357 */ { "btr"       , 2, { 0x0f, 0xb3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  358 */ { "lfs"       , 2, { 0x0f, 0xb4, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/*  359 */ { "lgs"       , 2, { 0x0f, 0xb5, 0x00, 0x00 }, X86_OPCODE_MODRM, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/*  360 */ { "movzx"     , 2, { 0x0f, 0xb6, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_byte, 1),
			},},
	/*  361 */ { "movzxw"   , 2, { 0x0f, 0xb7, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_word, 2),
			},},
	/*  362 */ { "btc"       , 2, { 0x0f, 0xbb, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  363 */ { "bsf"       , 2, { 0x0f, 0xbc, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  364 */ { "bsr"       , 2, { 0x0f, 0xbd, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_dword, 4),
			},},
	/*  365 */ { "movsx"     , 2, { 0x0f, 0xbe, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_byte, 1),
			},},
	/*  366 */ { "movsxw"   , 2, { 0x0f, 0xbf, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_word, 2),
			},},
	/*  367 */ { "xadd"      , 2, { 0x0f, 0xc0, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(2, register_byte, 1),
			},},
	/*  368 */ { "xadd"      , 2, { 0x0f, 0xc1, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  369 */ { "cmpps"     , 2, { 0x0f, 0xc2, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  370 */ { "movnti"    , 2, { 0x0f, 0xc3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_dword, 4),
			},},
	/*  371 */ { "pinsrw"    , 2, { 0x0f, 0xc4, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  372 */ { "pextrw"    , 2, { 0x0f, 0xc5, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_dword, 4),
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/*  373 */ { "shufps"    , 2, { 0x0f, 0xc6, 0x00, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  374 */ { "bswap"     , 2, { 0x0f, 0xc8, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(eax)),
			},},
	/*  375 */ { "bswap"     , 2, { 0x0f, 0xc9, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ecx)),
			},},
	/*  376 */ { "bswap"     , 2, { 0x0f, 0xca, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edx)),
			},},
	/*  377 */ { "bswap"     , 2, { 0x0f, 0xcb, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebx)),
			},},
	/*  378 */ { "bswap"     , 2, { 0x0f, 0xcc, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esp)),
			},},
	/*  379 */ { "bswap"     , 2, { 0x0f, 0xcd, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ebp)),
			},},
	/*  380 */ { "bswap"     , 2, { 0x0f, 0xce, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(esi)),
			},},
	/*  381 */ { "bswap"     , 2, { 0x0f, 0xcf, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(edi)),
			},},
	/*  382 */ { "psrlw"     , 2, { 0x0f, 0xd1, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  383 */ { "psrld"     , 2, { 0x0f, 0xd2, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  384 */ { "psrlq"     , 2, { 0x0f, 0xd3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  385 */ { "paddq"     , 2, { 0x0f, 0xd4, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  386 */ { "pmullw"    , 2, { 0x0f, 0xd5, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  387 */ { "pmovmskb"  , 2, { 0x0f, 0xd7, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(2, register_mmx, 8),
			},},
	/*  388 */ { "psubusb"   , 2, { 0x0f, 0xd8, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  389 */ { "psubusw"   , 2, { 0x0f, 0xd9, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  390 */ { "pminub"    , 2, { 0x0f, 0xda, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  391 */ { "pand"      , 2, { 0x0f, 0xdb, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  392 */ { "paddusb"   , 2, { 0x0f, 0xdc, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  393 */ { "paddusw"   , 2, { 0x0f, 0xdd, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  394 */ { "pmaxub"    , 2, { 0x0f, 0xde, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  395 */ { "pandn"     , 2, { 0x0f, 0xdf, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  396 */ { "pavgb"     , 2, { 0x0f, 0xe0, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  397 */ { "psraw"     , 2, { 0x0f, 0xe1, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  398 */ { "psrad"     , 2, { 0x0f, 0xe2, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  399 */ { "pavgw"     , 2, { 0x0f, 0xe3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  400 */ { "pmulhuw"   , 2, { 0x0f, 0xe4, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  401 */ { "pmulhw"    , 2, { 0x0f, 0xe5, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  402 */ { "movntq"    , 2, { 0x0f, 0xe7, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 8),
				X86_OP(2, register_xmm, 8),
			},},
	/*  403 */ { "psubsb"    , 2, { 0x0f, 0xe8, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  404 */ { "psubsw"    , 2, { 0x0f, 0xe9, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  405 */ { "pminsw"    , 2, { 0x0f, 0xea, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  406 */ { "por"       , 2, { 0x0f, 0xeb, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  407 */ { "paddsb"    , 2, { 0x0f, 0xec, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  408 */ { "paddsw"    , 2, { 0x0f, 0xed, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  409 */ { "pmaxsw"    , 2, { 0x0f, 0xee, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  410 */ { "pxor"      , 2, { 0x0f, 0xef, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  411 */ { "psllw"     , 2, { 0x0f, 0xf1, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  412 */ { "pslld"     , 2, { 0x0f, 0xf2, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  413 */ { "psllq"     , 2, { 0x0f, 0xf3, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  414 */ { "pmuludq"   , 2, { 0x0f, 0xf4, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  415 */ { "pmaddwd"   , 2, { 0x0f, 0xf5, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  416 */ { "psadbw"    , 2, { 0x0f, 0xf6, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  417 */ { "maskmovq"  , 2, { 0x0f, 0xf7, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  418 */ { "psubb"     , 2, { 0x0f, 0xf8, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  419 */ { "psubw"     , 2, { 0x0f, 0xf9, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  420 */ { "psubd"     , 2, { 0x0f, 0xfa, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  421 */ { "psubq"     , 2, { 0x0f, 0xfb, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  422 */ { "paddb"     , 2, { 0x0f, 0xfc, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  423 */ { "paddw"     , 2, { 0x0f, 0xfd, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  424 */ { "paddd"     , 2, { 0x0f, 0xfe, 0x00, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_mmx, 8),
			},},
	/*  425 */ { "movupd"    , 3, { 0x66, 0x0f, 0x10, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  426 */ { "movupd"    , 3, { 0x66, 0x0f, 0x11, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  427 */ { "movlpd"    , 3, { 0x66, 0x0f, 0x12, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 6),
			},},
	/*  428 */ { "movlpd"    , 3, { 0x66, 0x0f, 0x13, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  429 */ { "unpcklpd"  , 3, { 0x66, 0x0f, 0x14, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 8),
			},},
	/*  430 */ { "unpckhpd"  , 3, { 0x66, 0x0f, 0x15, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 8),
			},},
	/*  431 */ { "movhpd"    , 3, { 0x66, 0x0f, 0x16, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  432 */ { "movhpd"    , 3, { 0x66, 0x0f, 0x17, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 8),
				X86_OP(2, register_xmm, 8),
			},},
	/*  433 */ { "movapd"    , 3, { 0x66, 0x0f, 0x28, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  434 */ { "movapd"    , 3, { 0x66, 0x0f, 0x29, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  435 */ { "cvtpi2pd"  , 3, { 0x66, 0x0f, 0x2a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_mmx, 16),
			},},
	/*  436 */ { "movntpd"   , 3, { 0x66, 0x0f, 0x2b, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  437 */ { "cvttpd2pi" , 3, { 0x66, 0x0f, 0x2c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_mmx, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  438 */ { "cvtpd2pi"  , 3, { 0x66, 0x0f, 0x2d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_mmx, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  439 */ { "ucomisd"   , 3, { 0x66, 0x0f, 0x2e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  440 */ { "comisd"    , 3, { 0x66, 0x0f, 0x2f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  441 */ { "movmskpd"  , 3, { 0x66, 0x0f, 0x50, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_xmm, 16),
			},},
	/*  442 */ { "sqrtpd"    , 3, { 0x66, 0x0f, 0x51, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  443 */ { "andpd"     , 3, { 0x66, 0x0f, 0x54, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  444 */ { "andnpd"    , 3, { 0x66, 0x0f, 0x55, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  445 */ { "orpd"      , 3, { 0x66, 0x0f, 0x56, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  446 */ { "xorpd"     , 3, { 0x66, 0x0f, 0x57, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  447 */ { "addpd"     , 3, { 0x66, 0x0f, 0x58, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  448 */ { "mulpd"     , 3, { 0x66, 0x0f, 0x59, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  449 */ { "cvtpd2ps"  , 3, { 0x66, 0x0f, 0x5a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  450 */ { "cvtps2dq"  , 3, { 0x66, 0x0f, 0x5b, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  451 */ { "subpd"     , 3, { 0x66, 0x0f, 0x5c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  452 */ { "minpd"     , 3, { 0x66, 0x0f, 0x5d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  453 */ { "divpd"     , 3, { 0x66, 0x0f, 0x5e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  454 */ { "maxpd"     , 3, { 0x66, 0x0f, 0x5f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  455 */ { "punpcklbw" , 3, { 0x66, 0x0f, 0x60, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  456 */ { "punpcklwd" , 3, { 0x66, 0x0f, 0x61, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  457 */ { "punpckldq" , 3, { 0x66, 0x0f, 0x62, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  458 */ { "packsswb"  , 3, { 0x66, 0x0f, 0x63, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  459 */ { "pcmpgtb"   , 3, { 0x66, 0x0f, 0x64, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  460 */ { "pcmpgtw"   , 3, { 0x66, 0x0f, 0x65, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  461 */ { "pcmpgtd"   , 3, { 0x66, 0x0f, 0x66, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  462 */ { "packuswb"  , 3, { 0x66, 0x0f, 0x67, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  463 */ { "punpckhbw" , 3, { 0x66, 0x0f, 0x68, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(1, register_mmx, 16),
			},},
	/*  464 */ { "punpckhwd" , 3, { 0x66, 0x0f, 0x69, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(1, register_mmx, 16),
			},},
	/*  465 */ { "punpckhdq" , 3, { 0x66, 0x0f, 0x6a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(1, register_mmx, 16),
			},},
	/*  466 */ { "packssdw"  , 3, { 0x66, 0x0f, 0x6b, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(1, register_mmx, 16),
			},},
	/*  467 */ { "punpcklqdq", 3, { 0x66, 0x0f, 0x6c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  468 */ { "punpckhqdq", 3, { 0x66, 0x0f, 0x6d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  469 */ { "movd"      , 3, { 0x66, 0x0f, 0x6e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_dword, 4),
			},},
	/*  470 */ { "movdqa"    , 3, { 0x66, 0x0f, 0x6f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  471 */ { "pshufd"    , 3, { 0x66, 0x0f, 0x70, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  472 */ { "pcmpeqb"   , 3, { 0x66, 0x0f, 0x74, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  473 */ { "pcmpeqw"   , 3, { 0x66, 0x0f, 0x75, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  474 */ { "pcmpeqd"   , 3, { 0x66, 0x0f, 0x76, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  475 */ { "movd"      , 3, { 0x66, 0x0f, 0x7e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(2, register_xmm, 16),
			},},
	/*  476 */ { "movdqa"    , 3, { 0x66, 0x0f, 0x7f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  477 */ { "cmppd"     , 3, { 0x66, 0x0f, 0xc2, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  478 */ { "pinsrw"    , 3, { 0x66, 0x0f, 0xc4, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  479 */ { "pextrw"    , 3, { 0x66, 0x0f, 0xc5, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_dword, 4),
				X86_OP(2, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  480 */ { "shufpd"    , 3, { 0x66, 0x0f, 0xc6, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  481 */ { "psrlw"     , 3, { 0x66, 0x0f, 0xd1, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  482 */ { "psrld"     , 3, { 0x66, 0x0f, 0xd2, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  483 */ { "psrlq"     , 3, { 0x66, 0x0f, 0xd3, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  484 */ { "paddq"     , 3, { 0x66, 0x0f, 0xd4, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  485 */ { "pmullw"    , 3, { 0x66, 0x0f, 0xd5, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  486 */ { "movq"      , 3, { 0x66, 0x0f, 0xd6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 8),
				X86_OP(2, register_xmm, 8),
			},},
	/*  487 */ { "pmovmskb"  , 3, { 0x66, 0x0f, 0xd7, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(2, register_xmm, 16),
			},},
	/*  488 */ { "psubusb"   , 3, { 0x66, 0x0f, 0xd8, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  489 */ { "psubusw"   , 3, { 0x66, 0x0f, 0xd9, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  490 */ { "pminub"    , 3, { 0x66, 0x0f, 0xda, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  491 */ { "pand"      , 3, { 0x66, 0x0f, 0xdb, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  492 */ { "paddusb"   , 3, { 0x66, 0x0f, 0xdc, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  493 */ { "paddusw"   , 3, { 0x66, 0x0f, 0xdd, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  494 */ { "pmaxub"    , 3, { 0x66, 0x0f, 0xde, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  495 */ { "pandn"     , 3, { 0x66, 0x0f, 0xdf, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  496 */ { "pavgb"     , 3, { 0x66, 0x0f, 0xe0, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  497 */ { "psraw"     , 3, { 0x66, 0x0f, 0xe1, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  498 */ { "psrad"     , 3, { 0x66, 0x0f, 0xe2, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  499 */ { "pavgw"     , 3, { 0x66, 0x0f, 0xe3, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  500 */ { "pmulhuw"   , 3, { 0x66, 0x0f, 0xe4, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  501 */ { "pmulhw"    , 3, { 0x66, 0x0f, 0xe5, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  502 */ { "cvttpd2dq" , 3, { 0x66, 0x0f, 0xe6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  503 */ { "movntdq"   , 3, { 0x66, 0x0f, 0xe7, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  504 */ { "psubsb"    , 3, { 0x66, 0x0f, 0xe8, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  505 */ { "psubsw"    , 3, { 0x66, 0x0f, 0xe9, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  506 */ { "pminsw"    , 3, { 0x66, 0x0f, 0xea, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  507 */ { "por"       , 3, { 0x66, 0x0f, 0xeb, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  508 */ { "paddsb"    , 3, { 0x66, 0x0f, 0xec, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  509 */ { "paddsw"    , 3, { 0x66, 0x0f, 0xed, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  510 */ { "pmaxsw"    , 3, { 0x66, 0x0f, 0xee, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  511 */ { "pxor"      , 3, { 0x66, 0x0f, 0xef, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  512 */ { "psllw"     , 3, { 0x66, 0x0f, 0xf1, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  513 */ { "pslld"     , 3, { 0x66, 0x0f, 0xf2, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  514 */ { "psllq"     , 3, { 0x66, 0x0f, 0xf3, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  515 */ { "pmuludq"   , 3, { 0x66, 0x0f, 0xf4, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  516 */ { "pmaddwd"   , 3, { 0x66, 0x0f, 0xf5, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  517 */ { "psadbw"    , 3, { 0x66, 0x0f, 0xf6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  518 */ { "maskmovdqu", 3, { 0x66, 0x0f, 0xf7, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  519 */ { "psubb"     , 3, { 0x66, 0x0f, 0xf8, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  520 */ { "psubw"     , 3, { 0x66, 0x0f, 0xf9, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  521 */ { "psubd"     , 3, { 0x66, 0x0f, 0xfa, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  522 */ { "psubq"     , 3, { 0x66, 0x0f, 0xfb, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  523 */ { "paddb"     , 3, { 0x66, 0x0f, 0xfc, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  524 */ { "paddw"     , 3, { 0x66, 0x0f, 0xfd, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  525 */ { "paddd"     , 3, { 0x66, 0x0f, 0xfe, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  526 */ { "movsd"     , 3, { 0xf2, 0x0f, 0x10, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  527 */ { "movsd"     , 3, { 0xf2, 0x0f, 0x11, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  528 */ { "cvtsi2sd"  , 3, { 0xf2, 0x0f, 0x2a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_dword, 4),
			},},
	/*  529 */ { "cvttsd2si" , 3, { 0xf2, 0x0f, 0x2c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_xmm, 16),
			},},
	/*  530 */ { "cvtsd2si"  , 3, { 0xf2, 0x0f, 0x2d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_xmm, 16),
			},},
	/*  531 */ { "sqrtsd"    , 3, { 0xf2, 0x0f, 0x51, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  532 */ { "addsd"     , 3, { 0xf2, 0x0f, 0x58, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  533 */ { "mulsd"     , 3, { 0xf2, 0x0f, 0x59, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  534 */ { "cvtsd2ss"  , 3, { 0xf2, 0x0f, 0x5a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  535 */ { "subsd"     , 3, { 0xf2, 0x0f, 0x5c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  536 */ { "minsd"     , 3, { 0xf2, 0x0f, 0x5d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  537 */ { "divsd"     , 3, { 0xf2, 0x0f, 0x5e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  538 */ { "maxsd"     , 3, { 0xf2, 0x0f, 0x5f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  539 */ { "pshuflw"   , 3, { 0xf2, 0x0f, 0x70, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  540 */ { "cmpsd"     , 3, { 0xf2, 0x0f, 0xc2, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  541 */ { "movdq2q"   , 3, { 0xf2, 0x0f, 0xd6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  542 */ { "cvtpd2dq"  , 3, { 0xf2, 0x0f, 0xe6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  543 */ { "movss"     , 3, { 0xf3, 0x0f, 0x10, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  544 */ { "movss"     , 3, { 0xf3, 0x0f, 0x11, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  545 */ { "cvtsi2ss"  , 3, { 0xf3, 0x0f, 0x2a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_dword, 4),
			},},
	/*  546 */ { "cvttss2si" , 3, { 0xf3, 0x0f, 0x2c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_xmm, 16),
			},},
	/*  547 */ { "cvtss2si"  , 3, { 0xf3, 0x0f, 0x2d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_dword, 4),
				X86_OP(1, register_xmm, 16),
			},},
	/*  548 */ { "sqrtss"    , 3, { 0xf3, 0x0f, 0x51, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  549 */ { "rsqrtss"   , 3, { 0xf3, 0x0f, 0x52, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  550 */ { "rcpss"     , 3, { 0xf3, 0x0f, 0x53, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  551 */ { "addss"     , 3, { 0xf3, 0x0f, 0x58, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  552 */ { "mulss"     , 3, { 0xf3, 0x0f, 0x59, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  553 */ { "cvtss2sd"  , 3, { 0xf3, 0x0f, 0x5a, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  554 */ { "cvttps2dq" , 3, { 0xf3, 0x0f, 0x5b, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  555 */ { "subss"     , 3, { 0xf3, 0x0f, 0x5c, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  556 */ { "minss"     , 3, { 0xf3, 0x0f, 0x5d, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  557 */ { "divss"     , 3, { 0xf3, 0x0f, 0x5e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  558 */ { "maxss"     , 3, { 0xf3, 0x0f, 0x5f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  559 */ { "movdqu"    , 3, { 0xf3, 0x0f, 0x6f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  560 */ { "pshufhw"   , 3, { 0xf3, 0x0f, 0x71, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  561 */ { "movq"      , 3, { 0xf3, 0x0f, 0x7e, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 8),
				X86_OP(1, register_xmm, 8),
			},},
	/*  562 */ { "movdqu"    , 3, { 0xf3, 0x0f, 0x7f, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(2, register_xmm, 16),
			},},
	/*  563 */ { "cmpss"     , 3, { 0xf3, 0x0f, 0xc2, 0x00 }, X86_OPCODE_MODRM, 3, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/*  564 */ { "movq2dq"   , 3, { 0xf3, 0x0f, 0xd6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_mmx, 8),
			},},
	/*  565 */ { "cvtdq2pd"  , 3, { 0xf3, 0x0f, 0xe6, 0x00 }, X86_OPCODE_MODRM, 2, {
				X86_OP(2, register_xmm, 16),
				X86_OP(1, register_xmm, 16),
			},},
	/*  566 */ { "fadds"    , 2, { 0xd8, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  567 */ { "fmuls"    , 2, { 0xd8, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  568 */ { "fcoms"    , 2, { 0xd8, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  569 */ { "fcomps"   , 2, { 0xd8, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  570 */ { "fsubs"    , 2, { 0xd8, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  571 */ { "fsubrs"   , 2, { 0xd8, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  572 */ { "fdivs"    , 2, { 0xd8, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  573 */ { "fdivrs"   , 2, { 0xd8, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  574 */ { "fadd"      , 2, { 0xd8, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  575 */ { "fadd"      , 2, { 0xd8, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  576 */ { "fadd"      , 2, { 0xd8, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  577 */ { "fadd"      , 2, { 0xd8, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  578 */ { "fadd"      , 2, { 0xd8, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  579 */ { "fadd"      , 2, { 0xd8, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  580 */ { "fadd"      , 2, { 0xd8, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  581 */ { "fadd"      , 2, { 0xd8, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  582 */ { "fmul"      , 2, { 0xd8, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  583 */ { "fmul"      , 2, { 0xd8, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  584 */ { "fmul"      , 2, { 0xd8, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  585 */ { "fmul"      , 2, { 0xd8, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  586 */ { "fmul"      , 2, { 0xd8, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  587 */ { "fmul"      , 2, { 0xd8, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  588 */ { "fmul"      , 2, { 0xd8, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  589 */ { "fmul"      , 2, { 0xd8, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  590 */ { "fcom"      , 2, { 0xd8, 0xd0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  591 */ { "fcom"      , 2, { 0xd8, 0xd1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  592 */ { "fcom"      , 2, { 0xd8, 0xd2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  593 */ { "fcom"      , 2, { 0xd8, 0xd3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  594 */ { "fcom"      , 2, { 0xd8, 0xd4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  595 */ { "fcom"      , 2, { 0xd8, 0xd5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  596 */ { "fcom"      , 2, { 0xd8, 0xd6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  597 */ { "fcom"      , 2, { 0xd8, 0xd7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  598 */ { "fcomp"     , 2, { 0xd8, 0xd8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  599 */ { "fcomp"     , 2, { 0xd8, 0xd9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  600 */ { "fcomp"     , 2, { 0xd8, 0xda, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  601 */ { "fcomp"     , 2, { 0xd8, 0xdb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  602 */ { "fcomp"     , 2, { 0xd8, 0xdc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  603 */ { "fcomp"     , 2, { 0xd8, 0xdd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  604 */ { "fcomp"     , 2, { 0xd8, 0xde, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  605 */ { "fcomp"     , 2, { 0xd8, 0xdf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  606 */ { "fsub"      , 2, { 0xd8, 0xe0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  607 */ { "fsub"      , 2, { 0xd8, 0xe1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  608 */ { "fsub"      , 2, { 0xd8, 0xe2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  609 */ { "fsub"      , 2, { 0xd8, 0xe3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  610 */ { "fsub"      , 2, { 0xd8, 0xe4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  611 */ { "fsub"      , 2, { 0xd8, 0xe5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  612 */ { "fsub"      , 2, { 0xd8, 0xe6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  613 */ { "fsub"      , 2, { 0xd8, 0xe7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  614 */ { "fsubr"     , 2, { 0xd8, 0xe8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  615 */ { "fsubr"     , 2, { 0xd8, 0xe9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  616 */ { "fsubr"     , 2, { 0xd8, 0xea, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  617 */ { "fsubr"     , 2, { 0xd8, 0xeb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  618 */ { "fsubr"     , 2, { 0xd8, 0xec, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  619 */ { "fsubr"     , 2, { 0xd8, 0xed, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  620 */ { "fsubr"     , 2, { 0xd8, 0xee, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  621 */ { "fsubr"     , 2, { 0xd8, 0xef, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  622 */ { "fdiv"      , 2, { 0xd8, 0xf0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  623 */ { "fdiv"      , 2, { 0xd8, 0xf1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  624 */ { "fdiv"      , 2, { 0xd8, 0xf2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  625 */ { "fdiv"      , 2, { 0xd8, 0xf3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  626 */ { "fdiv"      , 2, { 0xd8, 0xf4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  627 */ { "fdiv"      , 2, { 0xd8, 0xf5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  628 */ { "fdiv"      , 2, { 0xd8, 0xf6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  629 */ { "fdiv"      , 2, { 0xd8, 0xf7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  630 */ { "fdivr"     , 2, { 0xd8, 0xf8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  631 */ { "fdivr"     , 2, { 0xd8, 0xf9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  632 */ { "fdivr"     , 2, { 0xd8, 0xfa, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  633 */ { "fdivr"     , 2, { 0xd8, 0xfb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  634 */ { "fdivr"     , 2, { 0xd8, 0xfc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  635 */ { "fdivr"     , 2, { 0xd8, 0xfd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  636 */ { "fdivr"     , 2, { 0xd8, 0xfe, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  637 */ { "fdivr"     , 2, { 0xd8, 0xff, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  638 */ { "flds"     , 2, { 0xd9, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  639 */ { "fsts"     , 2, { 0xd9, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  640 */ { "fstps"    , 2, { 0xd9, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  641 */ { "fldenv"    , 2, { 0xd9, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  642 */ { "fldcw"     , 2, { 0xd9, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  643 */ { "fstenv"    , 2, { 0xd9, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  644 */ { "fstcw"     , 2, { 0xd9, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  645 */ { "fld"       , 2, { 0xd9, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  646 */ { "fld"       , 2, { 0xd9, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  647 */ { "fld"       , 2, { 0xd9, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  648 */ { "fld"       , 2, { 0xd9, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  649 */ { "fld"       , 2, { 0xd9, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  650 */ { "fld"       , 2, { 0xd9, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  651 */ { "fld"       , 2, { 0xd9, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  652 */ { "fld"       , 2, { 0xd9, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  653 */ { "fxch"      , 2, { 0xd9, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  654 */ { "fxch"      , 2, { 0xd9, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  655 */ { "fxch"      , 2, { 0xd9, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  656 */ { "fxch"      , 2, { 0xd9, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  657 */ { "fxch"      , 2, { 0xd9, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  658 */ { "fxch"      , 2, { 0xd9, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  659 */ { "fxch"      , 2, { 0xd9, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  660 */ { "fxch"      , 2, { 0xd9, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  661 */ { "fnop"      , 2, { 0xd9, 0xd0, 0x00, 0x00 }, 0, 0, {}, },
	/*  662 */ { "fchs"      , 2, { 0xd9, 0xe0, 0x00, 0x00 }, 0, 0, {}, },
	/*  663 */ { "fabs"      , 2, { 0xd9, 0xe1, 0x00, 0x00 }, 0, 0, {}, },
	/*  664 */ { "ftst"      , 2, { 0xd9, 0xe4, 0x00, 0x00 }, 0, 0, {}, },
	/*  665 */ { "fxam"      , 2, { 0xd9, 0xe5, 0x00, 0x00 }, 0, 0, {}, },
	/*  666 */ { "fld1"      , 2, { 0xd9, 0xe8, 0x00, 0x00 }, 0, 0, {}, },
	/*  667 */ { "fldl2t"    , 2, { 0xd9, 0xe9, 0x00, 0x00 }, 0, 0, {}, },
	/*  668 */ { "fldl2e"    , 2, { 0xd9, 0xea, 0x00, 0x00 }, 0, 0, {}, },
	/*  669 */ { "fldpi"     , 2, { 0xd9, 0xeb, 0x00, 0x00 }, 0, 0, {}, },
	/*  670 */ { "fldlg2"    , 2, { 0xd9, 0xec, 0x00, 0x00 }, 0, 0, {}, },
	/*  671 */ { "fldln2"    , 2, { 0xd9, 0xed, 0x00, 0x00 }, 0, 0, {}, },
	/*  672 */ { "fldz"      , 2, { 0xd9, 0xee, 0x00, 0x00 }, 0, 0, {}, },
	/*  673 */ { "f2xm1"     , 2, { 0xd9, 0xf0, 0x00, 0x00 }, 0, 0, {}, },
	/*  674 */ { "fyl2x"     , 2, { 0xd9, 0xf1, 0x00, 0x00 }, 0, 0, {}, },
	/*  675 */ { "fptan"     , 2, { 0xd9, 0xf2, 0x00, 0x00 }, 0, 0, {}, },
	/*  676 */ { "fpatan"    , 2, { 0xd9, 0xf3, 0x00, 0x00 }, 0, 0, {}, },
	/*  677 */ { "fxtract"   , 2, { 0xd9, 0xf4, 0x00, 0x00 }, 0, 0, {}, },
	/*  678 */ { "fprem1"    , 2, { 0xd9, 0xf5, 0x00, 0x00 }, 0, 0, {}, },
	/*  679 */ { "fdecstp"   , 2, { 0xd9, 0xf6, 0x00, 0x00 }, 0, 0, {}, },
	/*  680 */ { "fincstp"   , 2, { 0xd9, 0xf7, 0x00, 0x00 }, 0, 0, {}, },
	/*  681 */ { "fprem"     , 2, { 0xd9, 0xf8, 0x00, 0x00 }, 0, 0, {}, },
	/*  682 */ { "fyl2xp1"   , 2, { 0xd9, 0xf9, 0x00, 0x00 }, 0, 0, {}, },
	/*  683 */ { "fsqrt"     , 2, { 0xd9, 0xfa, 0x00, 0x00 }, 0, 0, {}, },
	/*  684 */ { "fsincos"   , 2, { 0xd9, 0xfb, 0x00, 0x00 }, 0, 0, {}, },
	/*  685 */ { "frndint"   , 2, { 0xd9, 0xfc, 0x00, 0x00 }, 0, 0, {}, },
	/*  686 */ { "fscale"    , 2, { 0xd9, 0xfd, 0x00, 0x00 }, 0, 0, {}, },
	/*  687 */ { "fsin"      , 2, { 0xd9, 0xfe, 0x00, 0x00 }, 0, 0, {}, },
	/*  688 */ { "fcos"      , 2, { 0xd9, 0xff, 0x00, 0x00 }, 0, 0, {}, },
	/*  689 */ { "fiaddl"   , 2, { 0xda, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  690 */ { "fimull"   , 2, { 0xda, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  691 */ { "ficoml"   , 2, { 0xda, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  692 */ { "ficompl"  , 2, { 0xda, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  693 */ { "fisubl"   , 2, { 0xda, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  694 */ { "fisubrl"  , 2, { 0xda, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  695 */ { "fidivl"   , 2, { 0xda, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  696 */ { "fidivrl"  , 2, { 0xda, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  697 */ { "fcmovb"    , 2, { 0xda, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  698 */ { "fcmovb"    , 2, { 0xda, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  699 */ { "fcmovb"    , 2, { 0xda, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  700 */ { "fcmovb"    , 2, { 0xda, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  701 */ { "fcmovb"    , 2, { 0xda, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  702 */ { "fcmovb"    , 2, { 0xda, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  703 */ { "fcmovb"    , 2, { 0xda, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  704 */ { "fcmovb"    , 2, { 0xda, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  705 */ { "fcmove"    , 2, { 0xda, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  706 */ { "fcmove"    , 2, { 0xda, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  707 */ { "fcmove"    , 2, { 0xda, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  708 */ { "fcmove"    , 2, { 0xda, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  709 */ { "fcmove"    , 2, { 0xda, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  710 */ { "fcmove"    , 2, { 0xda, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  711 */ { "fcmove"    , 2, { 0xda, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  712 */ { "fcmove"    , 2, { 0xda, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  713 */ { "fcmovbe"   , 2, { 0xda, 0xd0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  714 */ { "fcmovbe"   , 2, { 0xda, 0xd1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  715 */ { "fcmovbe"   , 2, { 0xda, 0xd2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  716 */ { "fcmovbe"   , 2, { 0xda, 0xd3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  717 */ { "fcmovbe"   , 2, { 0xda, 0xd4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  718 */ { "fcmovbe"   , 2, { 0xda, 0xd5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  719 */ { "fcmovbe"   , 2, { 0xda, 0xd6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  720 */ { "fcmovbe"   , 2, { 0xda, 0xd7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  721 */ { "fcmovu"    , 2, { 0xda, 0xd8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  722 */ { "fcmovu"    , 2, { 0xda, 0xd9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  723 */ { "fcmovu"    , 2, { 0xda, 0xda, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  724 */ { "fcmovu"    , 2, { 0xda, 0xdb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  725 */ { "fcmovu"    , 2, { 0xda, 0xdc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  726 */ { "fcmovu"    , 2, { 0xda, 0xdd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  727 */ { "fcmovu"    , 2, { 0xda, 0xde, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  728 */ { "fcmovu"    , 2, { 0xda, 0xdf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  729 */ { "fucompp"   , 2, { 0xda, 0xe9, 0x00, 0x00 }, 0, 0, {}, },
	/*  730 */ { "fildl"    , 2, { 0xdb, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  731 */ { "fistl"    , 2, { 0xdb, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  732 */ { "fistpl"   , 2, { 0xdb, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  733 */ { "fldt"     , 2, { 0xdb, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 16),
			},},
	/*  734 */ { "fstpt"    , 2, { 0xdb, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 16),
			},},
	/*  735 */ { "fcmovnb"   , 2, { 0xdb, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  736 */ { "fcmovnb"   , 2, { 0xdb, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  737 */ { "fcmovnb"   , 2, { 0xdb, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  738 */ { "fcmovnb"   , 2, { 0xdb, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  739 */ { "fcmovnb"   , 2, { 0xdb, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  740 */ { "fcmovnb"   , 2, { 0xdb, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  741 */ { "fcmovnb"   , 2, { 0xdb, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  742 */ { "fcmovnb"   , 2, { 0xdb, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  743 */ { "fcmovne"   , 2, { 0xdb, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  744 */ { "fcmovne"   , 2, { 0xdb, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  745 */ { "fcmovne"   , 2, { 0xdb, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  746 */ { "fcmovne"   , 2, { 0xdb, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  747 */ { "fcmovne"   , 2, { 0xdb, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  748 */ { "fcmovne"   , 2, { 0xdb, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  749 */ { "fcmovne"   , 2, { 0xdb, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  750 */ { "fcmovne"   , 2, { 0xdb, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  751 */ { "fcmovnbe"  , 2, { 0xdb, 0xd0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  752 */ { "fcmovnbe"  , 2, { 0xdb, 0xd1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  753 */ { "fcmovnbe"  , 2, { 0xdb, 0xd2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  754 */ { "fcmovnbe"  , 2, { 0xdb, 0xd3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  755 */ { "fcmovnbe"  , 2, { 0xdb, 0xd4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  756 */ { "fcmovnbe"  , 2, { 0xdb, 0xd5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  757 */ { "fcmovnbe"  , 2, { 0xdb, 0xd6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  758 */ { "fcmovnbe"  , 2, { 0xdb, 0xd7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  759 */ { "fcmovnu"   , 2, { 0xdb, 0xd8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  760 */ { "fcmovnu"   , 2, { 0xdb, 0xd9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  761 */ { "fcmovnu"   , 2, { 0xdb, 0xda, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  762 */ { "fcmovnu"   , 2, { 0xdb, 0xdb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  763 */ { "fcmovnu"   , 2, { 0xdb, 0xdc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  764 */ { "fcmovnu"   , 2, { 0xdb, 0xdd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  765 */ { "fcmovnu"   , 2, { 0xdb, 0xde, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  766 */ { "fcmovnu"   , 2, { 0xdb, 0xdf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  767 */ { "fclex"     , 2, { 0xdb, 0xe2, 0x00, 0x00 }, 0, 0, {}, },
	/*  768 */ { "finit"     , 2, { 0xdb, 0xe3, 0x00, 0x00 }, 0, 0, {}, },
	/*  769 */ { "fucomi"    , 2, { 0xdb, 0xe8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  770 */ { "fucomi"    , 2, { 0xdb, 0xe9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  771 */ { "fucomi"    , 2, { 0xdb, 0xea, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  772 */ { "fucomi"    , 2, { 0xdb, 0xeb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  773 */ { "fucomi"    , 2, { 0xdb, 0xec, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  774 */ { "fucomi"    , 2, { 0xdb, 0xed, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  775 */ { "fucomi"    , 2, { 0xdb, 0xee, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  776 */ { "fucomi"    , 2, { 0xdb, 0xef, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  777 */ { "fcomi"     , 2, { 0xdb, 0xf0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  778 */ { "fcomi"     , 2, { 0xdb, 0xf1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  779 */ { "fcomi"     , 2, { 0xdb, 0xf2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  780 */ { "fcomi"     , 2, { 0xdb, 0xf3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  781 */ { "fcomi"     , 2, { 0xdb, 0xf4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  782 */ { "fcomi"     , 2, { 0xdb, 0xf5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  783 */ { "fcomi"     , 2, { 0xdb, 0xf6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  784 */ { "fcomi"     , 2, { 0xdb, 0xf7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  785 */ { "faddl"    , 2, { 0xdc, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  786 */ { "fmull"    , 2, { 0xdc, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  787 */ { "fcoml"    , 2, { 0xdc, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  788 */ { "fcompl"   , 2, { 0xdc, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  789 */ { "fsubl"    , 2, { 0xdc, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  790 */ { "fsubrl"   , 2, { 0xdc, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  791 */ { "fdivl"    , 2, { 0xdc, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  792 */ { "fdivrl"   , 2, { 0xdc, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  793 */ { "fadd"      , 2, { 0xdc, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  794 */ { "fadd"      , 2, { 0xdc, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  795 */ { "fadd"      , 2, { 0xdc, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  796 */ { "fadd"      , 2, { 0xdc, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  797 */ { "fadd"      , 2, { 0xdc, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  798 */ { "fadd"      , 2, { 0xdc, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  799 */ { "fadd"      , 2, { 0xdc, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  800 */ { "fadd"      , 2, { 0xdc, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  801 */ { "fmul"      , 2, { 0xdc, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  802 */ { "fmul"      , 2, { 0xdc, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  803 */ { "fmul"      , 2, { 0xdc, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  804 */ { "fmul"      , 2, { 0xdc, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  805 */ { "fmul"      , 2, { 0xdc, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  806 */ { "fmul"      , 2, { 0xdc, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  807 */ { "fmul"      , 2, { 0xdc, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  808 */ { "fmul"      , 2, { 0xdc, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  809 */ { "fsubr"     , 2, { 0xdc, 0xe0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  810 */ { "fsubr"     , 2, { 0xdc, 0xe1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  811 */ { "fsubr"     , 2, { 0xdc, 0xe2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  812 */ { "fsubr"     , 2, { 0xdc, 0xe3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  813 */ { "fsubr"     , 2, { 0xdc, 0xe4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  814 */ { "fsubr"     , 2, { 0xdc, 0xe5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  815 */ { "fsubr"     , 2, { 0xdc, 0xe6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  816 */ { "fsubr"     , 2, { 0xdc, 0xe7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  817 */ { "fsub"      , 2, { 0xdc, 0xe8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  818 */ { "fsub"      , 2, { 0xdc, 0xe9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  819 */ { "fsub"      , 2, { 0xdc, 0xea, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  820 */ { "fsub"      , 2, { 0xdc, 0xeb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  821 */ { "fsub"      , 2, { 0xdc, 0xec, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  822 */ { "fsub"      , 2, { 0xdc, 0xed, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  823 */ { "fsub"      , 2, { 0xdc, 0xee, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  824 */ { "fsub"      , 2, { 0xdc, 0xef, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  825 */ { "fdivr"     , 2, { 0xdc, 0xf0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  826 */ { "fdivr"     , 2, { 0xdc, 0xf1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  827 */ { "fdivr"     , 2, { 0xdc, 0xf2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  828 */ { "fdivr"     , 2, { 0xdc, 0xf3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  829 */ { "fdivr"     , 2, { 0xdc, 0xf4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  830 */ { "fdivr"     , 2, { 0xdc, 0xf5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  831 */ { "fdivr"     , 2, { 0xdc, 0xf6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  832 */ { "fdivr"     , 2, { 0xdc, 0xf7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  833 */ { "fdiv"      , 2, { 0xdc, 0xf8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  834 */ { "fdiv"      , 2, { 0xdc, 0xf9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  835 */ { "fdiv"      , 2, { 0xdc, 0xfa, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  836 */ { "fdiv"      , 2, { 0xdc, 0xfb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  837 */ { "fdiv"      , 2, { 0xdc, 0xfc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  838 */ { "fdiv"      , 2, { 0xdc, 0xfd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  839 */ { "fdiv"      , 2, { 0xdc, 0xfe, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  840 */ { "fdiv"      , 2, { 0xdc, 0xff, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  841 */ { "fldl"     , 2, { 0xdd, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  842 */ { "fstl"     , 2, { 0xdd, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  843 */ { "fstpl"    , 2, { 0xdd, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  844 */ { "frstor"    , 2, { 0xdd, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  845 */ { "fsave"     , 2, { 0xdd, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/*  846 */ { "fstsw"     , 2, { 0xdd, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  847 */ { "ffree"     , 2, { 0xdd, 0xc0, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  848 */ { "ffree"     , 2, { 0xdd, 0xc1, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  849 */ { "ffree"     , 2, { 0xdd, 0xc2, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  850 */ { "ffree"     , 2, { 0xdd, 0xc3, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  851 */ { "ffree"     , 2, { 0xdd, 0xc4, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  852 */ { "ffree"     , 2, { 0xdd, 0xc5, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  853 */ { "ffree"     , 2, { 0xdd, 0xc6, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  854 */ { "ffree"     , 2, { 0xdd, 0xc7, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  855 */ { "fst"       , 2, { 0xdd, 0xd0, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  856 */ { "fst"       , 2, { 0xdd, 0xd1, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  857 */ { "fst"       , 2, { 0xdd, 0xd2, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  858 */ { "fst"       , 2, { 0xdd, 0xd3, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  859 */ { "fst"       , 2, { 0xdd, 0xd4, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  860 */ { "fst"       , 2, { 0xdd, 0xd5, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  861 */ { "fst"       , 2, { 0xdd, 0xd6, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  862 */ { "fst"       , 2, { 0xdd, 0xd7, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  863 */ { "fstp"      , 2, { 0xdd, 0xd8, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  864 */ { "fstp"      , 2, { 0xdd, 0xd9, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  865 */ { "fstp"      , 2, { 0xdd, 0xda, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  866 */ { "fstp"      , 2, { 0xdd, 0xdb, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  867 */ { "fstp"      , 2, { 0xdd, 0xdc, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  868 */ { "fstp"      , 2, { 0xdd, 0xdd, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  869 */ { "fstp"      , 2, { 0xdd, 0xde, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  870 */ { "fstp"      , 2, { 0xdd, 0xdf, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  871 */ { "fucom"     , 2, { 0xdd, 0xe0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  872 */ { "fucom"     , 2, { 0xdd, 0xe1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  873 */ { "fucom"     , 2, { 0xdd, 0xe2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  874 */ { "fucom"     , 2, { 0xdd, 0xe3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  875 */ { "fucom"     , 2, { 0xdd, 0xe4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  876 */ { "fucom"     , 2, { 0xdd, 0xe5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  877 */ { "fucom"     , 2, { 0xdd, 0xe6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  878 */ { "fucom"     , 2, { 0xdd, 0xe7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  879 */ { "fucomp"    , 2, { 0xdd, 0xe8, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  880 */ { "fucomp"    , 2, { 0xdd, 0xe9, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  881 */ { "fucomp"    , 2, { 0xdd, 0xea, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  882 */ { "fucomp"    , 2, { 0xdd, 0xeb, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  883 */ { "fucomp"    , 2, { 0xdd, 0xec, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  884 */ { "fucomp"    , 2, { 0xdd, 0xed, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  885 */ { "fucomp"    , 2, { 0xdd, 0xee, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  886 */ { "fucomp"    , 2, { 0xdd, 0xef, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  887 */ { "fiadds"   , 2, { 0xde, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  888 */ { "fimuls"   , 2, { 0xde, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  889 */ { "ficoms"   , 2, { 0xde, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  890 */ { "ficomps"  , 2, { 0xde, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  891 */ { "fisubs"   , 2, { 0xde, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  892 */ { "fisubrs"  , 2, { 0xde, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  893 */ { "fidivs"   , 2, { 0xde, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  894 */ { "fidivrs"  , 2, { 0xde, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  895 */ { "faddp"     , 2, { 0xde, 0xc0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  896 */ { "faddp"     , 2, { 0xde, 0xc1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  897 */ { "faddp"     , 2, { 0xde, 0xc2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  898 */ { "faddp"     , 2, { 0xde, 0xc3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  899 */ { "faddp"     , 2, { 0xde, 0xc4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  900 */ { "faddp"     , 2, { 0xde, 0xc5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  901 */ { "faddp"     , 2, { 0xde, 0xc6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  902 */ { "faddp"     , 2, { 0xde, 0xc7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  903 */ { "fmulp"     , 2, { 0xde, 0xc8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  904 */ { "fmulp"     , 2, { 0xde, 0xc9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  905 */ { "fmulp"     , 2, { 0xde, 0xca, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  906 */ { "fmulp"     , 2, { 0xde, 0xcb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  907 */ { "fmulp"     , 2, { 0xde, 0xcc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  908 */ { "fmulp"     , 2, { 0xde, 0xcd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  909 */ { "fmulp"     , 2, { 0xde, 0xce, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  910 */ { "fmulp"     , 2, { 0xde, 0xcf, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  911 */ { "fcompp"    , 2, { 0xde, 0xd9, 0x00, 0x00 }, 0, 0, {}, },
	/*  912 */ { "fsubrp"    , 2, { 0xde, 0xe0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  913 */ { "fsubrp"    , 2, { 0xde, 0xe1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  914 */ { "fsubrp"    , 2, { 0xde, 0xe2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  915 */ { "fsubrp"    , 2, { 0xde, 0xe3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  916 */ { "fsubrp"    , 2, { 0xde, 0xe4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  917 */ { "fsubrp"    , 2, { 0xde, 0xe5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  918 */ { "fsubrp"    , 2, { 0xde, 0xe6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  919 */ { "fsubrp"    , 2, { 0xde, 0xe7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  920 */ { "fsubp"     , 2, { 0xde, 0xe8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  921 */ { "fsubp"     , 2, { 0xde, 0xe9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  922 */ { "fsubp"     , 2, { 0xde, 0xea, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  923 */ { "fsubp"     , 2, { 0xde, 0xeb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  924 */ { "fsubp"     , 2, { 0xde, 0xec, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  925 */ { "fsubp"     , 2, { 0xde, 0xed, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  926 */ { "fsubp"     , 2, { 0xde, 0xee, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  927 */ { "fsubp"     , 2, { 0xde, 0xef, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  928 */ { "fdivrp"    , 2, { 0xde, 0xf0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  929 */ { "fdivrp"    , 2, { 0xde, 0xf1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  930 */ { "fdivrp"    , 2, { 0xde, 0xf2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  931 */ { "fdivrp"    , 2, { 0xde, 0xf3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  932 */ { "fdivrp"    , 2, { 0xde, 0xf4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  933 */ { "fdivrp"    , 2, { 0xde, 0xf5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  934 */ { "fdivrp"    , 2, { 0xde, 0xf6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  935 */ { "fdivrp"    , 2, { 0xde, 0xf7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  936 */ { "fdivp"     , 2, { 0xde, 0xf8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  937 */ { "fdivp"     , 2, { 0xde, 0xf9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st1)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  938 */ { "fdivp"     , 2, { 0xde, 0xfa, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st2)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  939 */ { "fdivp"     , 2, { 0xde, 0xfb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st3)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  940 */ { "fdivp"     , 2, { 0xde, 0xfc, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st4)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  941 */ { "fdivp"     , 2, { 0xde, 0xfd, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st5)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  942 */ { "fdivp"     , 2, { 0xde, 0xfe, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st6)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  943 */ { "fdivp"     , 2, { 0xde, 0xff, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st7)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  944 */ { "filds"    , 2, { 0xdf, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  945 */ { "fists"    , 2, { 0xdf, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  946 */ { "fistps"   , 2, { 0xdf, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 2),
			},},
	/*  947 */ { "fbldt"    , 2, { 0xdf, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 16),
			},},
	/*  948 */ { "fildq"    , 2, { 0xdf, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  949 */ { "fbstp"     , 2, { 0xdf, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 16),
			},},
	/*  950 */ { "fistpq"   , 2, { 0xdf, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/*  951 */ { "fstsw"     , 2, { 0xdf, 0xe0, 0x00, 0x00 }, 0, 1, {
				X86_OP(0, register, X86_REGISTER(ax)),
			},},
	/*  952 */ { "fucomip"   , 2, { 0xdf, 0xe8, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  953 */ { "fucomip"   , 2, { 0xdf, 0xe9, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  954 */ { "fucomip"   , 2, { 0xdf, 0xea, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  955 */ { "fucomip"   , 2, { 0xdf, 0xeb, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  956 */ { "fucomip"   , 2, { 0xdf, 0xec, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  957 */ { "fucomip"   , 2, { 0xdf, 0xed, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  958 */ { "fucomip"   , 2, { 0xdf, 0xee, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  959 */ { "fucomip"   , 2, { 0xdf, 0xef, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  960 */ { "fcomip"    , 2, { 0xdf, 0xf0, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st0)),
			},},
	/*  961 */ { "fcomip"    , 2, { 0xdf, 0xf1, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st1)),
			},},
	/*  962 */ { "fcomip"    , 2, { 0xdf, 0xf2, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st2)),
			},},
	/*  963 */ { "fcomip"    , 2, { 0xdf, 0xf3, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st3)),
			},},
	/*  964 */ { "fcomip"    , 2, { 0xdf, 0xf4, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st4)),
			},},
	/*  965 */ { "fcomip"    , 2, { 0xdf, 0xf5, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st5)),
			},},
	/*  966 */ { "fcomip"    , 2, { 0xdf, 0xf6, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st6)),
			},},
	/*  967 */ { "fcomip"    , 2, { 0xdf, 0xf7, 0x00, 0x00 }, 0, 2, {
				X86_OP(0, register, X86_REGISTER(st0)),
				X86_OP(0, register, X86_REGISTER(st7)),
			},},
	/*  968 */ { "add"       , 2, { 0x80, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  969 */ { "or"        , 2, { 0x80, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  970 */ { "adc"       , 2, { 0x80, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  971 */ { "sbb"       , 2, { 0x80, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  972 */ { "and"       , 2, { 0x80, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  973 */ { "sub"       , 2, { 0x80, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  974 */ { "xor"       , 2, { 0x80, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  975 */ { "cmp"       , 2, { 0x80, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  976 */ { "add"       , 2, { 0x81, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  977 */ { "or"        , 2, { 0x81, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  978 */ { "adc"       , 2, { 0x81, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  979 */ { "sbb"       , 2, { 0x81, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  980 */ { "and"       , 2, { 0x81, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  981 */ { "sub"       , 2, { 0x81, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  982 */ { "xor"       , 2, { 0x81, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  983 */ { "cmp"       , 2, { 0x81, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/*  984 */ { "add"       , 2, { 0x83, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  985 */ { "or"        , 2, { 0x83, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  986 */ { "adc"       , 2, { 0x83, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  987 */ { "sbb"       , 2, { 0x83, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  988 */ { "and"       , 2, { 0x83, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  989 */ { "sub"       , 2, { 0x83, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  990 */ { "xor"       , 2, { 0x83, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  991 */ { "cmp"       , 2, { 0x83, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/*  992 */ { "rol"       , 2, { 0xc0, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  993 */ { "ror"       , 2, { 0xc0, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  994 */ { "rcl"       , 2, { 0xc0, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  995 */ { "rcr"       , 2, { 0xc0, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  996 */ { "shl"   , 2, { 0xc0, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  997 */ { "shr"       , 2, { 0xc0, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  998 */ { "sar"       , 2, { 0xc0, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/*  999 */ { "rol"       , 2, { 0xc1, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1000 */ { "ror"       , 2, { 0xc1, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1001 */ { "rcl"       , 2, { 0xc1, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1002 */ { "rcr"       , 2, { 0xc1, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1003 */ { "shl"   , 2, { 0xc1, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1004 */ { "shr"       , 2, { 0xc1, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1005 */ { "sar"       , 2, { 0xc1, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1006 */ { "rol"       , 2, { 0xd0, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1007 */ { "ror"       , 2, { 0xd0, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1008 */ { "rcl"       , 2, { 0xd0, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1009 */ { "rcr"       , 2, { 0xd0, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1010 */ { "shl"   , 2, { 0xd0, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1011 */ { "shr"       , 2, { 0xd0, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1012 */ { "sar"       , 2, { 0xd0, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1013 */ { "rol"       , 2, { 0xd1, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1014 */ { "ror"       , 2, { 0xd1, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1015 */ { "rcl"       , 2, { 0xd1, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1016 */ { "rcr"       , 2, { 0xd1, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1017 */ { "shl"   , 2, { 0xd1, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1018 */ { "shr"       , 2, { 0xd1, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1019 */ { "sar"       , 2, { 0xd1, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(1)),
			},},
	/* 1020 */ { "rol"       , 2, { 0xd2, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1021 */ { "ror"       , 2, { 0xd2, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1022 */ { "rcl"       , 2, { 0xd2, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1023 */ { "rcr"       , 2, { 0xd2, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1024 */ { "shl"   , 2, { 0xd2, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1025 */ { "shr"       , 2, { 0xd2, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1026 */ { "sar"       , 2, { 0xd2, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1027 */ { "rol"       , 2, { 0xd3, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1028 */ { "ror"       , 2, { 0xd3, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1029 */ { "rcl"       , 2, { 0xd3, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1030 */ { "rcr"       , 2, { 0xd3, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1031 */ { "shl"   , 2, { 0xd3, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1032 */ { "shr"       , 2, { 0xd3, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1033 */ { "sar"       , 2, { 0xd3, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, register, X86_REGISTER(cl)),
			},},
	/* 1034 */ { "test"      , 2, { 0xf6, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/* 1035 */ { "not"       , 2, { 0xf6, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1036 */ { "neg"       , 2, { 0xf6, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1037 */ { "mul"       , 2, { 0xf6, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1038 */ { "imul"      , 2, { 0xf6, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1039 */ { "div"       , 2, { 0xf6, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1040 */ { "idiv"      , 2, { 0xf6, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1041 */ { "test"      , 2, { 0xf7, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/* 1042 */ { "not"       , 2, { 0xf7, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1043 */ { "neg"       , 2, { 0xf7, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1044 */ { "mul"       , 2, { 0xf7, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1045 */ { "imul"      , 2, { 0xf7, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1046 */ { "div"       , 2, { 0xf7, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1047 */ { "idiv"      , 2, { 0xf7, 0x07, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1048 */ { "inc"       , 2, { 0xfe, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1049 */ { "dec"       , 2, { 0xfe, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_byte, 1),
			},},
	/* 1050 */ { "inc"       , 2, { 0xff, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1051 */ { "dec"       , 2, { 0xff, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1052 */ { "call"      , 2, { 0xff, 0x02, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1053 */ { "lcall"     , 2, { 0xff, 0x03, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1054 */ { "jmp"       , 2, { 0xff, 0x04, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1055 */ { "ljmp"      , 2, { 0xff, 0x05, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1056 */ { "push"      , 2, { 0xff, 0x06, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1057 */ { "sldt"      , 3, { 0x0f, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1058 */ { "str"       , 3, { 0x0f, 0x00, 0x01, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 4),
			},},
	/* 1059 */ { "lldt"      , 3, { 0x0f, 0x00, 0x02, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1060 */ { "ltr"       , 3, { 0x0f, 0x00, 0x03, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1061 */ { "verr"      , 3, { 0x0f, 0x00, 0x04, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1062 */ { "verw"      , 3, { 0x0f, 0x00, 0x05, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1063 */ { "sgdt"      , 3, { 0x0f, 0x01, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1064 */ { "sidt"      , 3, { 0x0f, 0x01, 0x01, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1065 */ { "lgdt"      , 3, { 0x0f, 0x01, 0x02, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1066 */ { "lidt"      , 3, { 0x0f, 0x01, 0x03, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 6),
			},},
	/* 1067 */ { "smsw"      , 3, { 0x0f, 0x01, 0x04, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1068 */ { "lmsw"      , 3, { 0x0f, 0x01, 0x06, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_word, 2),
			},},
	/* 1069 */ { "invlpg"    , 3, { 0x0f, 0x01, 0x07, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 1, {
				X86_OP(1, register_dword, 1),
			},},
	/* 1070 */ { "bt"        , 3, { 0x0f, 0xba, 0x04, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1071 */ { "bts"       , 3, { 0x0f, 0xba, 0x05, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1072 */ { "btr"       , 3, { 0x0f, 0xba, 0x06, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1073 */ { "btc"       , 3, { 0x0f, 0xba, 0x07, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 1),
			},},
	/* 1074 */ { "cmpxch8b" , 3, { 0x0f, 0xc7, 0x01, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 1, {
				X86_OP(1, register_dword, 8),
			},},
	/* 1075 */ { "mov"       , 2, { 0xc6, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_byte, 1),
				X86_OP(0, immediate, 1),
			},},
	/* 1076 */ { "mov"       , 2, { 0xc7, 0x00, 0x00, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED, 2, {
				X86_OP(1, register_dword, 4),
				X86_OP(0, immediate, 4),
			},},
	/* 1077 */ { "psrlw"    , 3, { 0x0f, 0x71, 0x02, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1078 */ { "psraw"    , 3, { 0x0f, 0x71, 0x04, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1079 */ { "psllw"    , 3, { 0x0f, 0x71, 0x06, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1080 */ { "psrlw"    , 4, { 0x66, 0x0f, 0x71, 0x02 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1081 */ { "psraw"    , 4, { 0x66, 0x0f, 0x71, 0x04 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1082 */ { "psllw"    , 4, { 0x66, 0x0f, 0x71, 0x06 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1083 */ { "psrld"    , 3, { 0x0f, 0x72, 0x02, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1084 */ { "psrad"    , 3, { 0x0f, 0x72, 0x04, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1085 */ { "pslld"    , 3, { 0x0f, 0x72, 0x06, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1086 */ { "psrld"    , 4, { 0x66, 0x0f, 0x72, 0x02 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1087 */ { "psrad"    , 4, { 0x66, 0x0f, 0x72, 0x04 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1088 */ { "pslld"    , 4, { 0x66, 0x0f, 0x72, 0x06 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1089 */ { "psrlq"    , 3, { 0x0f, 0x73, 0x02, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1090 */ { "psllq"    , 3, { 0x0f, 0x73, 0x06, 0x00 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(2, register_mmx, 8),
				X86_OP(0, immediate, 1),
			},},
	/* 1091 */ { "psrlq"    , 4, { 0x66, 0x0f, 0x73, 0x02 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1092 */ { "psrldq"   , 4, { 0x66, 0x0f, 0x73, 0x03 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1093 */ { "psllq"    , 4, { 0x66, 0x0f, 0x73, 0x06 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1094 */ { "pslldq"   , 4, { 0x66, 0x0f, 0x73, 0x07 }, X86_OPCODE_MODRM|X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 2, {
				X86_OP(1, register_xmm, 16),
				X86_OP(0, immediate, 1),
			},},
	/* 1095 */ { "fxsave"   , 3, { 0x0f, 0xae, 0x00, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1096 */ { "fxrstor"  , 3, { 0x0f, 0xae, 0x01, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1097 */ { "ldmxcsr"  , 3, { 0x0f, 0xae, 0x02, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1098 */ { "stmxcsr"  , 3, { 0x0f, 0xae, 0x03, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1099 */ { "lfence"   , 3, { 0x0f, 0xae, 0x05, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 0, {}, },
	/* 1100 */ { "mfence"   , 3, { 0x0f, 0xae, 0x06, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 0, {}, },
	/* 1101 */ { "sfence"   , 3, { 0x0f, 0xae, 0x07, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_PLUS, 0, {}, },
	/* 1102 */ { "clflush"  , 3, { 0x0f, 0xae, 0x07, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1103 */ { "prefetchnta", 3, { 0x0f, 0x18, 0x00, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1104 */ { "prefetcht0", 3, { 0x0f, 0x18, 0x01, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1105 */ { "prefetcht1", 3, { 0x0f, 0x18, 0x02, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
	/* 1106 */ { "prefetcht2", 3, { 0x0f, 0x18, 0x03, 0x00 }, X86_OPCODE_ENCODED|X86_OPCODE_MINUS, 0, {}, },
};
#endif

#endif /* !_AVS_X86_OPCODE_TABLE_H */

