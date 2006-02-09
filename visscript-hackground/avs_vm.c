#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_vm.h"

VM_INSTRUCTION(vm_insn_nop)
{

}

VM_INSTRUCTION(vm_insn_call)
{
	vmi->fn(obj, vmi->result, vmi->fnarg, vmi->fncount);
}

VM_INSTRUCTION(vm_insn_assign)
{
	*vmi->result = *vmi->arg[0] = *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_negate)
{
	*vmi->result = -(*vmi->arg[0]);
}

VM_INSTRUCTION(vm_insn_add)
{
	*vmi->result = *vmi->arg[0] + *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_sub)
{
	*vmi->result = *vmi->arg[0] - *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_mul)
{
	*vmi->result = *vmi->arg[0] * *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_div)
{
	*vmi->result = *vmi->arg[0] / *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_mod)
{
	*vmi->result = (int)*vmi->arg[0] % (int)*vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_and)
{
	*vmi->result = (int)*vmi->arg[0] & (int)*vmi->arg[1];

}

VM_INSTRUCTION(vm_insn_or)
{
	*vmi->result = (int)*vmi->arg[0] | (int)*vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_loadcounter)
{
	*vmi->result = *vmi->arg[0];
}

VM_INSTRUCTION(vm_insn_loop)
{
	*vmi->result -= 1.0;
	if (AVS_VALUEBOOL(*vmi->result))
		*next = vmi->jump[0];
}

VM_INSTRUCTION(vm_insn_condjump)
{
	if (AVS_VALUEBOOL(*vmi->arg[0]))
		*next = vmi->jump[0];
	else
		*next = vmi->jump[1];
}

VM_INSTRUCTION(vm_insn_jump)
{
	*next = vmi->jump[0];
}

VM_INSTRUCTION(vm_insn_loadptr)
{
	VM_RUNNABLEDATA(obj->pasm)->pointer = vmi->arg[0];
}

VM_INSTRUCTION(vm_insn_assigntoptr)
{
	*vmi->result = *VM_RUNNABLEDATA(obj->pasm)->pointer = *vmi->arg[0] = *vmi->arg[1];
}

VM_INSTRUCTION(vm_insn_assignfromptr)
{
	*vmi->result = *vmi->arg[0] = *VM_RUNNABLEDATA(obj->pasm)->pointer;
}

VmInstructionCall vm_instruction_table[] = {
	vm_insn_nop,
	vm_insn_call,
	vm_insn_assign,
	vm_insn_negate,
	vm_insn_add,
	vm_insn_sub,
	vm_insn_mul,
	vm_insn_div,
	vm_insn_mod,
	vm_insn_and,
	vm_insn_or, 
	vm_insn_loadcounter,
	vm_insn_loop,
	vm_insn_condjump,
	vm_insn_jump,
	vm_insn_loadptr,
	vm_insn_assigntoptr,
	vm_insn_assignfromptr,
};

#ifdef AVS_DEBUG
void vm_dump(VmInstruction *vmi)
{
	char *name[] = { "nop", "call", "assign", "negate", "add", "sub",
			 "mul", "div", "mod", "and", "or", "ldcounter",
			 "loop", "condjmp", "jump", "ldptr", "assigntoptr",
			 "assignfromptr", "unknown" };     
	int i, x;

	for (i=0; i < sizeof(vm_instruction_table) / sizeof(VmInstructionCall); i++)
		if (vm_instruction_table[i] == vmi->run)
			break;

	fprintf(stderr, "VM: %p: [0x%08x]\t%-10s", vmi, (int)vmi->result, name[i]);
	
	switch (i) {
		case AvsInsnCall:
			fprintf(stderr, "%p [", vmi->fn);
			if (!vmi->fncount)
				fprintf(stderr, "empty");
			else 
				for (x=0; x < vmi->fncount; x++)
					fprintf(stderr, "%p (#%.2f), ", vmi->fnarg[x], *vmi->fnarg[x]);
			fprintf(stderr, "]");
			break;
			
		case AvsInsnAssign:
			fprintf(stderr, "%p (#%.2f), %p (#%.2f), %p (#%.2f)", vmi->result, *vmi->result, 
									  vmi->arg[0], *vmi->arg[0], 
									  vmi->arg[1], *vmi->arg[1]);
			break;

		case AvsInsnLoadCounter:
		case AvsInsnNegate:
			fprintf(stderr, "%p (#%.2f)", vmi->arg[0],*vmi->arg[0]);
			break;
		
		case AvsInsnCondJump:
			fprintf(stderr, "%p (#%.2f), true(*%p), false(*%p)", vmi->arg[0],*vmi->arg[0], vmi->jump[0], vmi->jump[1]);
			break;
				
		case AvsInsnLoop:
		case AvsInsnJump:
			fprintf(stderr, "*%p", vmi->jump[0]);
			break;

		case AvsInsnAssignToPtr:
		case AvsInsnAssignFromPtr:
		case AvsInsnLoadPtr:
		case AvsInsnNop:
			break;

		default:
			fprintf(stderr, "%p (#%.2f), %p (#%.2f)", vmi->arg[0], *vmi->arg[0], 
								vmi->arg[1], *vmi->arg[1]);
			break;
	}

	fputc('\n', stderr);
}

int vm_dumptree(VmRunnableData *vm)
{
	VmInstruction *insn;
	
	for (insn=vm->ix.base; insn != NULL; insn = insn->next) 
		vm_dump(insn);
	
	return 0;
}
#endif

int vm_run(AvsRunnable *obj)
{
	VmRunnableData *vm;
	VmInstruction *insn, *next;

	vm = VM_RUNNABLEDATA(obj->pasm);
	avs_debug(print("VM: Runnable data: %p insn = %p, last = %p", vm, vm->ix.base, vm->ix.end));
	avs_debug(call(vm_dumptree(vm)));
	avs_debug(print("VM: End of execution tree..."));
	avs_debug(print("VM: Running..."));
	
	for (insn=vm->ix.base; insn != NULL; insn = next) {
		avs_debug(call(vm_dump(insn)));
		next = insn->next;
		insn->run(obj, insn, &next);
	}

	avs_debug(print("VM: Halting..."));
	return VISUAL_OK;
}
