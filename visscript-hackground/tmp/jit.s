#VM: 0x8057618: [0x0805760c]     assign    0x805760c (#0.00), 0x80575ec (#0.00), 0x80575fc (#0.00)
#VM: 0x8057688: [0x0805767c]     add       0x80575ec (#0.00), 0x805766c (#1.00)
#VM: 0x80576d8: [0x080576cc]     mul       0x805767c (#0.00), 0x80576bc (#5.00)
#VM: 0x8057728: [0x0805771c]     assign    0x805771c (#0.00), 0x805770c (#10.00), 0x80576cc (#0.00)
.data
constant1:
	.double 1
constant5:
	.double 5
x:
	.double 0
y:
	.double 10
lint:
	.long 0
fmt:
	.string "Y: %.2f\n"
.text
.globl main
main:
	# x=0;y=(x+1)*5;
	movl $0, %eax
	movl %eax, (x)
	flds (x)
	fldl (x)
	faddl (constant1)
	faddl (%eax)
	fdivl (constant5)
	fmull (constant5)
	fadds (constant1)
	fmuls (constant5)
	fdivs (constant5)
	fstl (y)
	fsts (y)
	fstl 4(%esp)
	movl $fmt, (%esp)
	call printf
	movl $1, %eax
	int $0x80
