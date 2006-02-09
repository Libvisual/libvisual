#ifndef _AVS_VM_INSTRUCTION_H
#define _AVS_VM_INSTRUCTION_H 1

/**
 * Calculate the number of used contexts since last marker.
 *
 * @param gd Global data context. 
 *
 * @return Number of used context since last marker.
 */
static inline int avs_vm_instruction_context_count(VmGlobalData *gd)
{
	return (gd->ix - gd->ixmarker) + 1;
}

/* prototypes */
VmInstructionContext *avs_vm_instruction_context_pop(VmGlobalData *gd);
int avs_vm_instruction_context_mark(VmGlobalData *gd, int nestlevel);
int avs_vm_instruction_context_add(VmGlobalData *gd);
int avs_vm_instruction_merge(VmGlobalData *gd, VmInstructionContext *ctx);
int avs_vm_instruction_link(VmGlobalData *gd, VmInstruction *insn);
int avs_vm_instruction_pop(VmGlobalData *vm, VmInstruction *insn);
VmInstruction *avs_vm_instruction_new(VmGlobalData *vm, AvsInstruction insn, AvsNumber *retval, AvsNumber *arg0, AvsNumber *arg1);
VmInstruction *avs_vm_instruction_add(VmGlobalData *gd, AvsInstruction insn, AvsNumber *retval, AvsNumber *arg0, AvsNumber *arg1);
int avs_vm_instruction_reset(VmGlobalData *gd);
int avs_vm_instruction_cleanup(VmGlobalData *gd);
int avs_vm_instruction_init(VmGlobalData *gd);

#endif /* !_AVS_VM_INSTRUCTION_H */
