.code64
.text

.global gdc
.type gdc, %function
gdc: # %rdi = factor
	push %rbp
	mov %rsp, %rbp

	xor %rax, %rax

	pop %rbp
	retq
.size gdc, .-gdc
