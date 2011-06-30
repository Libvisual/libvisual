#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define AVS_X86_OPCODE_TABLE 1
#include "avs.h"
#include "avs_x86_opcode.h"

#define PIDFILE "/tmp/visscript.mem"
#define MAXFILESIZE 1024*1024

struct emit_argument {
	unsigned int	index;
	unsigned int	type;
	unsigned int	value;
	unsigned int	disp, base;
	unsigned int	sib_base, sib_index, sib_scale;
};

static inline unsigned char *request_buf(X86Context *ctx, int size)
{
	unsigned char *retval;
	
    if(ctx->position + size > MAXFILESIZE)
        return NULL;

	retval = ctx->buf + ctx->position;
	ctx->position += size;
	return retval;
}

static int inline sizeof_value(unsigned int value)
{
	if (value < 0x1)
		return 0;
	if (value < 0x100)
		return 1;
	if (value < 0x10000)
		return 2;	
	return 4;
}

static inline unsigned char * encode_number(X86Context *ctx, unsigned int value, unsigned int size)
{
	unsigned char *buf;
	
	if ((buf=request_buf(ctx, size)) == NULL)
		return NULL;
	
#ifdef __LITTLE_ENDIAN
	memcpy(buf, &value, size);
#else
	{
		unsigned char pbuf[4];
		pbuf[0] = value;
		pbuf[1] = value >> 8;
		pbuf[2] = value >> 16;
		pbuf[3] = value >> 24;
		memcpy(buf, pbuf, size);
	}
#endif

	return buf;
}

static void inline encode_modrm(unsigned char *modrm, X86OpcodeModReg mode, int value)
{
	if (!modrm)
		return;

	switch (mode) {
		case x86_modrm_reg:
			*modrm &= ~X86_MODRM_MASK_REG;
			*modrm |= (value << X86_MODRM_SH_REG) & X86_MODRM_MASK_REG;
			break;
			
		case x86_modrm_mod:
			*modrm &= ~X86_MODRM_MASK_MOD;
			*modrm |= (value << X86_MODRM_SH_MOD) & X86_MODRM_MASK_MOD;
			break;
			
		case x86_modrm_rm:
			*modrm &= ~X86_MODRM_MASK_RM;
			*modrm |= value & X86_MODRM_MASK_RM;
			break;

		default:
			break;
	}
}

static void inline encode_opcode(unsigned char *modrm, int opcode)
{
	encode_modrm(modrm, x86_modrm_reg, opcode);
}

static void inline encode_register(unsigned char *modrm, X86OpcodeModReg mode, int value)
{
	#define _R(x) X86_REGISTER(x)
	static unsigned char translate[x86_opcode_last] = {
		 [_R(eax)]  = 0, [_R(ecx)]  = 1, [_R(edx)]  = 2, [_R(ebx)]  = 3, 
		 [_R(esp)]  = 4, [_R(ebp)]  = 5, [_R(esi)]  = 6, [_R(edi)]  = 7, 
		 [_R(ax)]   = 0, [_R(cx)]   = 1, [_R(dx)]   = 2, [_R(bx)]   = 3,   
		 [_R(sp)]   = 4, [_R(bp)]   = 5, [_R(si)]   = 6, [_R(di)]   = 7, 
		 [_R(al)]   = 0, [_R(cl)]   = 1, [_R(dl)]   = 2, [_R(bl)]   = 3,   
		 [_R(ah)]   = 4, [_R(ch)]   = 5, [_R(dh)]   = 6, [_R(bh)]   = 7, 
		 [_R(mm0)]  = 0, [_R(mm1)]  = 1, [_R(mm2)]  = 2, [_R(mm3)]  = 3,  
		 [_R(mm4)]  = 4, [_R(mm5)]  = 5, [_R(mm6)]  = 6, [_R(mm7)]  = 7, 
		 [_R(xmm0)] = 0, [_R(xmm1)] = 1, [_R(xmm2)] = 2, [_R(xmm3)] = 3, 
		 [_R(xmm4)] = 4, [_R(xmm5)] = 5, [_R(xmm6)] = 6, [_R(xmm7)] = 7, 
		 [_R(cr0)]  = 0,                 [_R(cr2)]  = 2, [_R(cr3)]  = 3, 
		 [_R(cr4)]  = 4, 
		 [_R(dr0)]  = 0, [_R(dr1)]  = 1, [_R(dr2)]  = 2, [_R(dr3)]  = 3, 
		                                 [_R(dr6)]  = 6, [_R(dr7)]  = 7, 
	};
	#undef _R

	value = translate[value];
	switch (mode) {
		case x86_modrm_reg:
			encode_modrm(modrm, x86_modrm_reg, value);
			break;
			
		case x86_modrm_mod:
			encode_modrm(modrm, x86_modrm_rm, value);
			encode_modrm(modrm, x86_modrm_mod, 0x03); /* Select register table */
			break;

		default:
			break;
	}
}

static void inline encode_sib(unsigned char *sib, X86OpcodeSib mode, int value)
{
	if (!sib)
		return;

	switch (mode) {
		case x86_sib_ss:
			*sib &= ~X86_SIB_MASK_SS;
			*sib |= (value << X86_SIB_SH_SS) & X86_SIB_MASK_SS;
			break;
			
		case x86_sib_index:
			*sib &= ~X86_SIB_MASK_INDEX;
			*sib |= (value << X86_SIB_SH_INDEX) & X86_SIB_MASK_INDEX;
			break;
			
		case x86_sib_base:
			*sib &= ~X86_SIB_MASK_BASE;
			*sib |= value & X86_SIB_MASK_BASE;
			break;

		default:
			break;
	}
}

static void inline encode_scale(X86Context *ctx, 
				unsigned char *modrm, 
				struct emit_argument *arg, 
				int *dispsize)
{
	#define _R(x) X86_REGISTER(x)
	static unsigned char translate[x86_opcode_last] = {
		 [_R(eax)]  = 1, [_R(ecx)]  = 2, [_R(edx)]  = 3, [_R(ebx)]  = 4, 
		                 [_R(ebp)]  = 6, [_R(esi)]  = 7, [_R(edi)]  = 8, 
	};
	static unsigned char translate_base[x86_opcode_last] = {
		 [_R(eax)]  = 1, [_R(ecx)]  = 2, [_R(edx)]  = 3, [_R(ebx)]  = 4, 
		 [_R(esp)]  = 5,                 [_R(esi)]  = 7, [_R(edi)]  = 8, 
	};
	#undef _R
	static unsigned char sib_ss[9] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, };
	unsigned char *sib;
	int sindex, sbase;
	
	sindex = translate[arg->sib_index];
	sbase = translate_base[arg->sib_base];
	if ((sib = request_buf(ctx, 1)) == NULL)
		return;
	*sib = 0;

	if (arg->sib_scale > 8) {
		avs_debug(log("X86: Scale overflow!"));
		arg->sib_scale = 8;
	}

	/* No index given, set index to none */
	if (!sindex)
		sindex = 5; /* Select empty index slot */

	/* No base given, set base to none.
	 * Set modrm modifier to '[--]', otherwise EBP will be used as base.
	 * A disp32 value is expected.  */
	if (!sbase) {
		sbase = 6; /* Select empty base slot */
		switch (arg->sib_base) {
			case X86_REGISTER(ebp): 
				/* EBP given as base */
				break;

			case X86_REGISTER(none):
				/* No base given, change modrm */
				encode_modrm(modrm, x86_modrm_mod, 0);
				
				if (dispsize) {
					/* Caller will handle disp32 */
					*dispsize = 4; 
				} else {
					encode_number(ctx, 0, 4);
				}
				break;
		}
	}
	
	encode_sib(sib, x86_sib_ss, sib_ss[arg->sib_scale]);
	encode_sib(sib, x86_sib_index, sindex - 1);
	encode_sib(sib, x86_sib_base, sbase - 1);
	encode_modrm(modrm, x86_modrm_rm, 4); /* Select SIB byte */
}

static void inline encode_displacement(X86Context *ctx, unsigned char *modrm, struct emit_argument *arg)
{
	#define _R(x) X86_REGISTER(x)
	static unsigned char translate[x86_opcode_last] = {
		 [_R(eax)]  = 1, [_R(ecx)]  = 2, [_R(edx)]  = 3, [_R(ebx)]  = 4, 
		                 [_R(ebp)]  = 6, [_R(esi)]  = 7, [_R(edi)]  = 8, 
	};
	#undef _R
	static unsigned char modifier[5] = { 0, 1, 2, 2, 2, };
	int rm, size;

	rm = translate[arg->base];
	size = sizeof_value(arg->disp);

	avs_debug(print("X86: Index rm: %d size: %d displacement: 0x%08x", rm, size, arg->disp));

	if (rm)
		encode_modrm(modrm, x86_modrm_rm, rm - 1);

	encode_modrm(modrm, x86_modrm_mod, modifier[size]);
	switch (arg->base) {
		case X86_REGISTER(ebp): 
			/* EBP register can only be selected as base with a displacement value.
			 * if no displacement value has been given, encode it with a
			 * zero byte displacement */
			if (!size) {
				size = 1;
				encode_modrm(modrm, x86_modrm_mod, modifier[size]);
			}
			break;
		
		case X86_REGISTER(esp):
			/* ESP register can only be selected as base when scaled */
			arg->sib_base = X86_REGISTER(esp);
			arg->sib_index = X86_REGISTER(none);
			arg->sib_scale = 0;

			/* fallthrough */
		case x86_opcode_scale:
			encode_scale(ctx, modrm, arg, &size);
			break;

		default:
			if (!rm) {
				avs_debug(log("X86: Invalid displacement register specified!"));
				break;
			}
	}

	if (size) {
		encode_number(ctx, arg->disp, size);
	}
}

static inline void emit_opcode_argument(X86Context *ctx,
					X86OpcodeTable *opc, 
					unsigned char *modrm, 
					struct emit_argument *arg)
{
	struct X86OpcodeOperand *op = &opc->op[arg->index];
	
	switch (op->type) {
		case x86_opcode_immediate:
			encode_number(ctx, arg->value, op->size);
			break;

		case x86_opcode_relative:
			encode_number(ctx, arg->value - (int)(ctx->buf + ctx->opcode_position) - opc->length - 4, 4); /* oprsize */
			encode_modrm(modrm, x86_modrm_rm, 5); /* select disp32 am */
			break;

		case x86_opcode_offset:
			encode_number(ctx, arg->value, 4); /* oprsize */
			encode_modrm(modrm, x86_modrm_rm, 5); /* select disp32 am */
			break;

		case x86_opcode_register:
			break; /* ignore */

		case x86_opcode_register_byte:
		case x86_opcode_register_word:
		case x86_opcode_register_dword:
		case x86_opcode_register_xmm:
		case x86_opcode_register_mmx:
		case x86_opcode_register_control:
		case x86_opcode_register_debug:
		case x86_opcode_register_segment:
			/* can be a register, or a memory offset */
			switch (arg->type) {
				case x86_opcode_offset:
					encode_number(ctx, arg->value, 4); /* oprsize */
					encode_modrm(modrm, x86_modrm_rm, 5); /* select disp32 am */
					break;

				case x86_opcode_register:
					encode_register(modrm, op->mode, arg->value);
					break;

				case x86_opcode_disp:
					encode_displacement(ctx, modrm, arg);
					break;

				case x86_opcode_scale:
					encode_scale(ctx, modrm, arg, NULL);
					break;
					
			}
			break;
	}


}

void x86_emit_opcode(X86Context *ctx, int op, ...)
{
	X86OpcodeTable *opc = opcodetable + op;
	unsigned char *code, modrm_save, *modrm = &modrm_save;
	struct emit_argument arg;
	va_list ap;
	int i;

	avs_debug(print("X86: Emitting opcode: %s (%d)", opc->name, op));
	ctx->opcode_position = ctx->position;

	/* Copy opcode into code buffer */
	if ((code=request_buf(ctx, opc->length)) == NULL)
		return;

	memcpy(code, opc->opcode, opc->length);

	if (opc->flags & X86_OPCODE_ENCODED) {
		modrm = code + opc->length - 1;
		*modrm = 0;
		encode_opcode(modrm, opc->opcode[opc->length-1]);
	} else {
		if (opc->flags & X86_OPCODE_MODRM) {
			if ((modrm=request_buf(ctx,1)) == NULL)
				return;
		}	
		*modrm = 0;
	}

	if (opc->flags & X86_OPCODE_PLUS)
		encode_modrm(modrm, x86_modrm_mod, 0x3);
			
	va_start(ap, op);
	for (i=0; i < opc->opcount; i++) {
		arg.index = i;
		arg.type = va_arg(ap, int);

		switch (arg.type) {
			case x86_opcode_disp:
				arg.disp = va_arg(ap, int);
				arg.base = va_arg(ap, int);

				if (arg.base != x86_opcode_scale) {
					arg.base = va_arg(ap, int); /* register */
					break;
				}
				/* fallthrough */
				
			case x86_opcode_scale:
				arg.sib_base = va_arg(ap, int);
				arg.sib_index = va_arg(ap, int);
				arg.sib_scale = va_arg(ap, int);
				break;
				
			default:
				arg.value = va_arg(ap, int);
				break;
		}

		avs_debug(print("X86: Argument: index = %d, type = %d, "
				"value = 0x%08x, disp = 0x%08x, base = %d",
				arg.index, arg.type, arg.value, arg.disp, arg.base));

		ctx->offset[i] = ctx->position;
		emit_opcode_argument(ctx, opc, modrm, &arg);
	}
	va_end(ap);
}

static int context_dtor(VisObject *object)
{
    char file[512];
	X86Context *ctx = AVS_X86_CONTEXT(object);

    sprintf(file, "%s.%d", PIDFILE, getpid());
    close(ctx->fd);
    unlink(file);

	return 0; 
}

static int context_ctor(X86Context *ctx)
{
    char file[512];

	memset(ctx, 0, sizeof(X86Context));
	
    snprintf(file, sizeof(file), "%s.%d", PIDFILE, getpid());

    if((ctx->fd = open(file, O_RDWR | O_CREAT, (mode_t)0600)) < 0) {
        avs_debug(error("open() failed"));
        return -VISUAL_ERROR_GENERAL;
    }

    if(lseek(ctx->fd, MAXFILESIZE-1, SEEK_SET) < 0) {
        close(ctx->fd);
        avs_debug(error("Unable to expand file"));
        return -VISUAL_ERROR_GENERAL;
    }

    if(write(ctx->fd, "", 1) != 1) {
        close(ctx->fd);
        avs_debug(error("Unable to write to file"));
        return -VISUAL_ERROR_GENERAL;
    }

    ctx->buf = mmap(0, MAXFILESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, ctx->fd, 0);

    if(ctx->buf == MAP_FAILED) {
        close(ctx->fd);
        avs_debug(error("Unable to map memory"));
        return -VISUAL_ERROR_GENERAL;
    }
		
	ctx->position = 0;

	return VISUAL_OK;
}

/**
 */
unsigned char * x86_argument_offset(X86Context *ctx, unsigned int index)
{
	return ctx->buf + ctx->offset[index & 3];
}

unsigned char * x86_next_offset(X86Context *ctx)
{
	return ctx->buf + ctx->position;
}

/**
 * Reset a X86 context structure
 * 
 * @param ctx X86 context to reset
 *
 * @returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int x86_context_reset(X86Context *ctx)
{
	ctx->position = 0;
	return VISUAL_OK;
}

/**
 * Initialize a X86 context structure
 * 
 * @param ctx X86 context to initialize
 *
 * @returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int x86_context_init(X86Context *ctx)
{
	visual_object_initialize(VISUAL_OBJECT(ctx), FALSE, context_dtor);
	return context_ctor(ctx);
}

/**
 * Create and initialize a X86 context structure
 *
 * @returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
X86Context * x86_context_new()
{
	X86Context *ctx;

	/* Allocate memory for X86 Context Object */
	if ((ctx = visual_mem_new0(X86Context, 1)) == NULL)
		return NULL;

	visual_object_initialize(VISUAL_OBJECT(ctx), TRUE, context_dtor);

	/* Initialize X86 Context Object */
	if (context_ctor(ctx) != VISUAL_OK) {
		visual_object_unref(VISUAL_OBJECT(ctx));
		return NULL;
	}
	
	return ctx;
}

