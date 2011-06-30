#ifndef _AVS_X86_OPCODE_H
#define _AVS_X86_OPCODE_H 1

#define AVS_X86_CONTEXT(obj)		(VISUAL_CHECK_CAST ((obj), X86Context))

struct _X86Context;
typedef struct _X86Context X86Context;
struct _X86OpcodeTable;
typedef struct _X86OpcodeTable X86OpcodeTable;
enum _X86OpcodeModReg;
typedef enum _X86OpcodeModReg X86OpcodeModReg;
enum _X86OpcodeOperandType;
typedef enum _X86OpcodeOperandType X86OpcodeOperandType;
enum _X86OpcodeSib;
typedef enum _X86OpcodeSib X86OpcodeSib;

typedef unsigned int X86OpcodeArgumentOffset;

struct _X86Context {
	VisObject		object;
    int                 fd;
	unsigned char		*buf;
	unsigned int		length;
	unsigned int		length_max;
	unsigned int		position;
	unsigned int		opcode_position;
	X86OpcodeArgumentOffset offset[4];
};

enum _X86OpcodeModReg {
	x86_modrm_none,
	x86_modrm_mod,
	x86_modrm_reg,
	x86_modrm_rm,
};

enum _X86OpcodeSib {
	x86_sib_none,
	x86_sib_ss,
	x86_sib_index,
	x86_sib_base,
};

enum _X86OpcodeOperandType {
	x86_opcode_ignored,
	x86_opcode_register_static_base,
	x86_opcode_register_st0,
	x86_opcode_register_st1,
	x86_opcode_register_st2,
	x86_opcode_register_st3,
	x86_opcode_register_st4,
	x86_opcode_register_st5,
	x86_opcode_register_st6,
	x86_opcode_register_st7,
	x86_opcode_register_mm0,
	x86_opcode_register_mm1,
	x86_opcode_register_mm2,
	x86_opcode_register_mm3,
	x86_opcode_register_mm4,
	x86_opcode_register_mm5,
	x86_opcode_register_mm6,
	x86_opcode_register_mm7,
	x86_opcode_register_xmm0,
	x86_opcode_register_xmm1,
	x86_opcode_register_xmm2,
	x86_opcode_register_xmm3,
	x86_opcode_register_xmm4,
	x86_opcode_register_xmm5,
	x86_opcode_register_xmm6,
	x86_opcode_register_xmm7,
	x86_opcode_register_cr0,
	x86_opcode_register_cr2,
	x86_opcode_register_cr3,
	x86_opcode_register_cr4,
	x86_opcode_register_dr0,
	x86_opcode_register_dr1,
	x86_opcode_register_dr2,
	x86_opcode_register_dr3,
	x86_opcode_register_dr6,
	x86_opcode_register_dr7,
	x86_opcode_register_ax,
	x86_opcode_register_bx,
	x86_opcode_register_cx,
	x86_opcode_register_dx,
	x86_opcode_register_sp,
	x86_opcode_register_bp,
	x86_opcode_register_si,
	x86_opcode_register_di,
	x86_opcode_register_eax,
	x86_opcode_register_ebx,
	x86_opcode_register_ecx,
	x86_opcode_register_edx,
	x86_opcode_register_esi,
	x86_opcode_register_edi,
	x86_opcode_register_ebp,
	x86_opcode_register_esp,
	x86_opcode_register_eip,
	x86_opcode_register_al,
	x86_opcode_register_ah,
	x86_opcode_register_bl,
	x86_opcode_register_bh,
	x86_opcode_register_cl,
	x86_opcode_register_ch,
	x86_opcode_register_dl,
	x86_opcode_register_dh,
	x86_opcode_register_cs,
	x86_opcode_register_ds,
	x86_opcode_register_es,
	x86_opcode_register_fs,
	x86_opcode_register_gs,
	x86_opcode_register_ss,
	x86_opcode_register_1,
	x86_opcode_register_static_end,
	x86_opcode_register,
	x86_opcode_register_dword,
	x86_opcode_register_word,
	x86_opcode_register_byte,
	x86_opcode_register_mmx,
	x86_opcode_register_xmm,
	x86_opcode_register_control,
	x86_opcode_register_debug,
	x86_opcode_register_segment,
	x86_opcode_register_dssi,
	x86_opcode_register_esdi,
	x86_opcode_register_none,
	x86_opcode_relative,
	x86_opcode_immediate,
	x86_opcode_offset,
	x86_opcode_disp,
	x86_opcode_scale,
	x86_opcode_last,
};

#define X86_OPCODE_MODRM 	0x0001
#define X86_OPCODE_ENCODED	0x0002
#define X86_OPCODE_MINUS	0x0004
#define X86_OPCODE_PLUS		0x0008

struct _X86OpcodeTable {
	char		*name;
	unsigned char	length;
	unsigned char	opcode[4];
	unsigned char	flags;
	unsigned char	opcount;
	struct X86OpcodeOperand {
		unsigned char mode;
		unsigned char type;
		unsigned char size;
	} op[4];
};

#define X86_REGISTER(reg)	  x86_opcode_register_##reg
#define X86_OP(modrm, mode, size) { modrm, x86_opcode_##mode, size, }
#define X86_MODRM_MASK_REG	0x38
#define X86_MODRM_MASK_MOD	0xc0
#define X86_MODRM_MASK_RM	0x07
#define X86_MODRM_SH_REG	3
#define X86_MODRM_SH_MOD	6
#define X86_MODRM_SH_RM		0

#define X86_SIB_MASK_SS		0xc0
#define X86_SIB_MASK_INDEX	0x38
#define X86_SIB_MASK_BASE	0x07

#define X86_SIB_SH_SS		6
#define X86_SIB_SH_INDEX	3
#define X86_SIB_SH_BASE		0

#define x86_glue(a, b)			a ## b
#define x86_xglue(a, b)			x86_glue(a, b)

#define x86_type_st0 _register_st0
#define x86_type_st1 _register_st1
#define x86_type_st2 _register_st2
#define x86_type_st3 _register_st3
#define x86_type_st4 _register_st4
#define x86_type_st5 _register_st5
#define x86_type_st6 _register_st6
#define x86_type_st7 _register_st7
#define x86_type_mm0 _register_mm0
#define x86_type_mm1 _register_mm1
#define x86_type_mm2 _register_mm2
#define x86_type_mm3 _register_mm3
#define x86_type_mm4 _register_mm4
#define x86_type_mm5 _register_mm5
#define x86_type_mm6 _register_mm6
#define x86_type_mm7 _register_mm7
#define x86_type_xmm0 _register_xmm0
#define x86_type_xmm1 _register_xmm1
#define x86_type_xmm2 _register_xmm2
#define x86_type_xmm3 _register_xmm3
#define x86_type_xmm4 _register_xmm4
#define x86_type_xmm5 _register_xmm5
#define x86_type_xmm6 _register_xmm6
#define x86_type_xmm7 _register_xmm7
#define x86_type_cr0 _register_cr0
#define x86_type_cr2 _register_cr2
#define x86_type_cr3 _register_cr3
#define x86_type_cr4 _register_cr4
#define x86_type_dr0 _register_dr0
#define x86_type_dr1 _register_dr1
#define x86_type_dr2 _register_dr2
#define x86_type_dr3 _register_dr3
#define x86_type_dr6 _register_dr6
#define x86_type_dr7 _register_dr7
#define x86_type_ax _register_ax
#define x86_type_bx _register_bx
#define x86_type_cx _register_cx
#define x86_type_dx _register_dx
#define x86_type_sp _register_sp
#define x86_type_bp _register_bp
#define x86_type_si _register_si
#define x86_type_di _register_di
#define x86_type_eax _register_eax
#define x86_type_ebx _register_ebx
#define x86_type_ecx _register_ecx
#define x86_type_edx _register_edx
#define x86_type_esi _register_esi
#define x86_type_edi _register_edi
#define x86_type_ebp _register_ebp
#define x86_type_esp _register_esp
#define x86_type_eip _register_eip
#define x86_type_al _register_al
#define x86_type_ah _register_ah
#define x86_type_bl _register_bl
#define x86_type_bh _register_bh
#define x86_type_cl _register_cl
#define x86_type_ch _register_ch
#define x86_type_dl _register_dl
#define x86_type_dh _register_dh
#define x86_type_cs _register_cs
#define x86_type_ds _register_ds
#define x86_type_es _register_es
#define x86_type_fs _register_fs
#define x86_type_gs _register_gs
#define x86_type_ss _register_ss
#define x86_type_1 _register_1
#define x86_type_dssi _register_dssi
#define x86_type_esdi _register_esdi


#define x86_type_reg(arg)	_register_##arg
#define x86_type_imm(arg)	_immediate
#define x86_type_immediate(arg)	_immediate

#define x86_value_reg(arg)	x86_opcode_register, X86_REGISTER(arg)
#define x86_value_imm(arg)	x86_opcode_immediate, (int)arg
#define x86_value_immediate(arg)	x86_opcode_immediate, (int)arg

#define x86_value_byte(arg)	x86_opcode_immediate, (int)arg
#define x86_value_word(arg)	x86_opcode_immediate, (int)arg
#define x86_value_long(arg)	x86_opcode_immediate, (int)arg

#define x86_type_offset(arg)	_offset
#define x86_type_relative(arg)	_relative

#define x86_value_offset(arg)	x86_opcode_offset, (int)arg
#define x86_value_relative(arg)	x86_opcode_offset, (int)arg

#define x86_type_none
#define x86_value_none

#define x86_type_disp(disp, base)		_scale
#define x86_type_scale(scale, index, base)	_scale

#define x86_value_disp(d, base)		x86_opcode_disp, \
					d, \
					x86_value_ ## base 

#define x86_value_scale(b, i, s)		x86_opcode_scale, \
					x86_opcode_register_ ## b, \
					x86_opcode_register_ ## i, \
					s

#define x86_value_st0 x86_opcode_register, X86_REGISTER(st0)
#define x86_value_st1 x86_opcode_register, X86_REGISTER(st1)
#define x86_value_st2 x86_opcode_register, X86_REGISTER(st2)
#define x86_value_st3 x86_opcode_register, X86_REGISTER(st3)
#define x86_value_st4 x86_opcode_register, X86_REGISTER(st4)
#define x86_value_st5 x86_opcode_register, X86_REGISTER(st5)
#define x86_value_st6 x86_opcode_register, X86_REGISTER(st6)
#define x86_value_st7 x86_opcode_register, X86_REGISTER(st7)
#define x86_value_mm0 x86_opcode_register, X86_REGISTER(mm0)
#define x86_value_mm1 x86_opcode_register, X86_REGISTER(mm1)
#define x86_value_mm2 x86_opcode_register, X86_REGISTER(mm2)
#define x86_value_mm3 x86_opcode_register, X86_REGISTER(mm3)
#define x86_value_mm4 x86_opcode_register, X86_REGISTER(mm4)
#define x86_value_mm5 x86_opcode_register, X86_REGISTER(mm5)
#define x86_value_mm6 x86_opcode_register, X86_REGISTER(mm6)
#define x86_value_mm7 x86_opcode_register, X86_REGISTER(mm7)
#define x86_value_xmm0 x86_opcode_register, X86_REGISTER(xmm0)
#define x86_value_xmm1 x86_opcode_register, X86_REGISTER(xmm1)
#define x86_value_xmm2 x86_opcode_register, X86_REGISTER(xmm2)
#define x86_value_xmm3 x86_opcode_register, X86_REGISTER(xmm3)
#define x86_value_xmm4 x86_opcode_register, X86_REGISTER(xmm4)
#define x86_value_xmm5 x86_opcode_register, X86_REGISTER(xmm5)
#define x86_value_xmm6 x86_opcode_register, X86_REGISTER(xmm6)
#define x86_value_xmm7 x86_opcode_register, X86_REGISTER(xmm7)
#define x86_value_cr0 x86_opcode_register, X86_REGISTER(cr0)
#define x86_value_cr2 x86_opcode_register, X86_REGISTER(cr2)
#define x86_value_cr3 x86_opcode_register, X86_REGISTER(cr3)
#define x86_value_cr4 x86_opcode_register, X86_REGISTER(cr4)
#define x86_value_dr0 x86_opcode_register, X86_REGISTER(dr0)
#define x86_value_dr1 x86_opcode_register, X86_REGISTER(dr1)
#define x86_value_dr2 x86_opcode_register, X86_REGISTER(dr2)
#define x86_value_dr3 x86_opcode_register, X86_REGISTER(dr3)
#define x86_value_dr6 x86_opcode_register, X86_REGISTER(dr6)
#define x86_value_dr7 x86_opcode_register, X86_REGISTER(dr7)
#define x86_value_ax x86_opcode_register, X86_REGISTER(ax)
#define x86_value_bx x86_opcode_register, X86_REGISTER(bx)
#define x86_value_cx x86_opcode_register, X86_REGISTER(cx)
#define x86_value_dx x86_opcode_register, X86_REGISTER(dx)
#define x86_value_sp x86_opcode_register, X86_REGISTER(sp)
#define x86_value_bp x86_opcode_register, X86_REGISTER(bp)
#define x86_value_si x86_opcode_register, X86_REGISTER(si)
#define x86_value_di x86_opcode_register, X86_REGISTER(di)
#define x86_value_eax x86_opcode_register, X86_REGISTER(eax)
#define x86_value_ebx x86_opcode_register, X86_REGISTER(ebx)
#define x86_value_ecx x86_opcode_register, X86_REGISTER(ecx)
#define x86_value_edx x86_opcode_register, X86_REGISTER(edx)
#define x86_value_esi x86_opcode_register, X86_REGISTER(esi)
#define x86_value_edi x86_opcode_register, X86_REGISTER(edi)
#define x86_value_ebp x86_opcode_register, X86_REGISTER(ebp)
#define x86_value_esp x86_opcode_register, X86_REGISTER(esp)
#define x86_value_eip x86_opcode_register, X86_REGISTER(eip)
#define x86_value_al x86_opcode_register, X86_REGISTER(al)
#define x86_value_ah x86_opcode_register, X86_REGISTER(ah)
#define x86_value_bl x86_opcode_register, X86_REGISTER(bl)
#define x86_value_bh x86_opcode_register, X86_REGISTER(bh)
#define x86_value_cl x86_opcode_register, X86_REGISTER(cl)
#define x86_value_ch x86_opcode_register, X86_REGISTER(ch)
#define x86_value_dl x86_opcode_register, X86_REGISTER(dl)
#define x86_value_dh x86_opcode_register, X86_REGISTER(dh)
#define x86_value_cs x86_opcode_register, X86_REGISTER(cs)
#define x86_value_ds x86_opcode_register, X86_REGISTER(ds)
#define x86_value_es x86_opcode_register, X86_REGISTER(es)
#define x86_value_fs x86_opcode_register, X86_REGISTER(fs)
#define x86_value_gs x86_opcode_register, X86_REGISTER(gs)
#define x86_value_ss x86_opcode_register, X86_REGISTER(ss)
#define x86_value_1 x86_opcode_register, X86_REGISTER(1)
#define x86_value_dssi x86_opcode_register, X86_REGISTER(dssi)
#define x86_value_esdi x86_opcode_register, X86_REGISTER(esdi)

#define x86_opprefix(insn)			x86_glue(x86_opcode_, insn)
#define x86_link0(insn, arg0)			x86_xglue(insn, arg0)
#define x86_link1(insn, arg0, arg1)		x86_xglue(x86_xglue(insn, arg0), arg1)
#define x86_link2(insn, arg0, arg1, arg2)	x86_xglue(x86_xglue(x86_xglue(insn, arg0), arg1), arg2)

#define x86_emit0(ctx, insn)			x86_emit_opcode(ctx, x86_opprefix(insn))
#define x86_emit1(ctx, insn, arg0) 		x86_emit_opcode(ctx, \
							x86_link0(x86_opprefix(insn), x86_xglue(x86_type_, arg0)), \
							x86_xglue(x86_value_, arg0))
#define x86_emit2(ctx, insn, arg0, arg1)	x86_emit_opcode(ctx, \
							x86_link1(x86_opprefix(insn), x86_xglue(x86_type_, arg1), \
									              x86_xglue(x86_type_, arg0)), \
							x86_xglue(x86_value_, arg1), \
							x86_xglue(x86_value_, arg0))
#define x86_emit3(ctx, insn, arg0, arg1, arg2) x86_emit_opcode(ctx, \
							x86_link2(x86_opprefix(insn), x86_xglue(x86_type_, arg2), \
									              x86_xglue(x86_type_, arg1), \
									              x86_xglue(x86_type_, arg0)), \
							x86_xglue(x86_value_, arg2), \
							x86_xglue(x86_value_, arg1), \
							x86_xglue(x86_value_, arg0))

#include "avs_x86_opcode_table.h"

/* prototypes */
void x86_emit_opcode(X86Context *ctx, int op, ...);
unsigned char *x86_argument_offset(X86Context *ctx, unsigned int index);
unsigned char *x86_next_offset(X86Context *ctx);
int x86_context_reset(X86Context *ctx);
int x86_context_init(X86Context *ctx);
X86Context *x86_context_new(void);

#endif /* !_AVS_X86_OPCODE_H */
