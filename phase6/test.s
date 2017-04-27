_main:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$main.size, %esp

	.data
.L2:
	.asciz	"hello world\n"
	.text
	leal	.L2,	%eax
	movl	%eax,-4(%ebp)
	movl	-4(%ebp), %eax
	movl	%eax, 0(%esp)
	call	_printf
	movl	%eax,	-8(%ebp)

.L1:
	movl	%ebp,	%esp
	popl	%ebp
	ret

	.globl	_main
	.set	main.size, 24

