#ifndef _AVS_IL_REGISTER_H
#define _AVS_IL_REGISTER_H 1

typedef enum _AvsILRegisterType {
	ILRegisterTypeNull,
	ILRegisterTypeConstant,
	ILRegisterTypeVariable,
	ILRegisterTypeInvalid
} ILRegisterType;

#define ILRegisterPointer	0x00000001

typedef struct _AvsILRegister {
	int			ref;
	ILRegisterType		type;
	unsigned int		flags;
	
	union {
		AvsNumber		constant;
		AvsRunnableVariable	*variable;
	} value;

	/* Private pointer for compilers to use */
	void			*private;
} ILRegister;

#endif /* !_AVS_IL_REGISTER_H */
