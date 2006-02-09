#ifndef _AVS_IL_REGISTER_H
#define _AVS_IL_REGISTER_H 1

typedef enum _AvsILRegisterType {
	ILRegisterTypeNull,
	ILRegisterTypeConstant,
	ILRegisterTypeVariable,
	ILRegisterTypeInvalid
} ILRegisterType;

typedef struct _AvsILRegister {
	int			ref;
	ILRegisterType		type;

	union {
		AvsNumber		constant;
		AvsRunnableVariable	*variable;
	} value;
} ILRegister;

#endif /* !_AVS_IL_REGISTER_H */
