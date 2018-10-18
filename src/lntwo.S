.code64
.text

.global lntwo
.type lntwo, %function
lntwo: # %rdi = factor
	push %rbp
	mov %rsp, %rbp

	bsr %rdi, %rax

	pop %rbp
	retq
.size lntwo, .-lntwo
