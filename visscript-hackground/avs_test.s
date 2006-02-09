	.file	"avs_test.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"x"
.LC1:
	.string	"y"
	.section	.rodata.str1.32,"aMS",@progbits,1
	.align 32
.LC4:
	.string	"VM: var_x (%p) = %f var_y (%p) = %f\n"
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC3:
	.long	1092616192
	.text
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	leal	12(%ebp), %edx
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$32828, %esp
	xorl	%ebx, %ebx
	andl	$-16, %esp
	movl	%edx, 4(%esp)
	leal	8(%ebp), %edx
	movl	%edx, (%esp)
	call	visual_init
	movl	$3, (%esp)
	call	visual_log_set_verboseness
	cmpl	$1, 8(%ebp)
	jg	.L3
.L2:
	movl	%ebx, (%esp)
	leal	-32792(%ebp), %esi
	movl	$32767, %edi
	movl	%edi, 8(%esp)
	leal	-32796(%ebp), %ebx
	movl	%esi, 4(%esp)
	call	read
	xorl	%ecx, %ecx
	movl	%eax, %edi
	movb	%cl, -32792(%eax,%ebp)
	call	avs_runnable_context_new
	movl	%eax, -32804(%ebp)
	movl	%eax, (%esp)
	call	avs_runnable_new
	movl	%eax, (%esp)
	movl	$.LC0, %edx
	movl	%eax, %esi
	movl	%edx, 4(%esp)
	movl	%ebx, 8(%esp)
	leal	-32800(%ebp), %ebx
	call	avs_runnable_variable_bind
	movl	%ebx, 8(%esp)
	movl	$.LC1, %eax
	movl	%esi, (%esp)
	movl	%eax, 4(%esp)
	call	avs_runnable_variable_bind
	movl	%edi, 8(%esp)
	leal	-32792(%ebp), %ecx
	movl	%ecx, 4(%esp)
	movl	%esi, (%esp)
	call	avs_runnable_compile
	flds	.LC3
	movl	$.LC4, %eax
	movl	%ebx, 20(%esp)
	movl	stderr, %edi
	fldz
	fstl	12(%esp)
	leal	-32796(%ebp), %edx
	fstps	-32796(%ebp)
	fsts	-32800(%ebp)
	fstpl	24(%esp)
	movl	%edx, 8(%esp)
	movl	%edi, (%esp)
	movl	%eax, 4(%esp)
	call	fprintf
	movl	%esi, (%esp)
	call	avs_runnable_execute
	flds	-32800(%ebp)
	movl	$.LC4, %ecx
	movl	%ebx, 20(%esp)
	movl	stderr, %edx
	leal	-32796(%ebp), %ebx
	movl	%ecx, 4(%esp)
	fstpl	24(%esp)
	flds	-32796(%ebp)
	movl	%edx, (%esp)
	movl	%ebx, 8(%esp)
	fstpl	12(%esp)
	call	fprintf
	movl	%esi, (%esp)
	call	visual_object_unref
	movl	-32804(%ebp), %edi
	movl	%edi, (%esp)
	call	visual_object_unref
	leal	-12(%ebp), %esp
	xorl	%eax, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.p2align 4,,7
.L3:
	movl	12(%ebp), %ebx
	xorl	%esi, %esi
	movl	%esi, 4(%esp)
	movl	4(%ebx), %ecx
	movl	%ecx, (%esp)
	call	open
	movl	%eax, %ebx
	jmp	.L2
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.3.5 (Debian 1:3.3.5-8)"
