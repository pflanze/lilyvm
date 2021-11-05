	.text
	.file	"fib.c"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	cmpl	$2, %edi
	jne	.LBB0_2
# %bb.1:
	movq	8(%rsi), %rdi
	xorl	%esi, %esi
	movl	$10, %edx
	callq	strtol
	movq	%rax, %rbx
	movl	%ebx, %edi
	callq	fib
	movl	$.L.str.2, %edi
	movq	%rbx, %rsi
	movl	%eax, %edx
	xorl	%eax, %eax
	callq	printf
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB0_2:
	.cfi_def_cfa_offset 16
	movl	$.L.str, %edi
	movl	$.L.str.1, %esi
	movl	$.L__PRETTY_FUNCTION__.main, %ecx
	movl	$20, %edx
	callq	__assert_fail
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90                         # -- Begin function fib
	.type	fib,@function
fib:                                    # @fib
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	pushq	%rbx
	pushq	%rax
	movl	$1, %ebp
	cmpl	$2, %edi
	jl	.LBB1_4
# %bb.1:
	movl	%edi, %ebx
	addl	$2, %ebx
	xorl	%ebp, %ebp
	.p2align	4, 0x90
.LBB1_2:                                # =>This Inner Loop Header: Depth=1
	leal	-3(%rbx), %edi
	callq	fib
	addl	%eax, %ebp
	addl	$-2, %ebx
	cmpl	$3, %ebx
	jg	.LBB1_2
# %bb.3:
	addl	$1, %ebp
.LBB1_4:
	movl	%ebp, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end1:
	.size	fib, .Lfunc_end1-fib
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"argc == 2"
	.size	.L.str, 10

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"fib.c"
	.size	.L.str.1, 6

	.type	.L__PRETTY_FUNCTION__.main,@object # @__PRETTY_FUNCTION__.main
.L__PRETTY_FUNCTION__.main:
	.asciz	"int main(int, const char **)"
	.size	.L__PRETTY_FUNCTION__.main, 29

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"fib(%li) = %i\n"
	.size	.L.str.2, 15

	.section	".note.GNU-stack","",@progbits
	.addrsig
