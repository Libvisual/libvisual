#include <stdio.h>
#include <stdlib.h>

#include "avs.h"

AVS_STACK_MANAGER_TEMPLATE_LOCAL(AvsNumber, sm_num, 1024, 1024*1024);

int main(void)
{
	AvsStackManager *sm;
	AvsNumber *p;
	int i, size = 0, ns, ft;

	sm = sm_num_new();
	ft = (1024*2048) / (1024*1024);
	for (i=0; i < 1024*2048; i++) {
		p = sm_num_push(sm);
		ns = sm->stack->end - sm->stack->base;
		if (ns != size) {
			fprintf(stderr, "Stack resize: %d -> %d (ptr: %d) (elem: %d)\n",size, ns, sm->stack->end - sm->stack->pointer,  sm->stack->elem_count);
			size = ns;
		}
		if (p == NULL) {
			fprintf(stderr, "WTF?: %d\n", i);
			exit(1);
		}
		*p = 10.0;
		if (i % ft == 0)
			sm_num_pop(sm, p);
	}
	return 0;
}
