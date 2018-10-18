.code64
.text

.global gcd
.type gcd, %function
gcd: # %rdi = factor
	push %rbp
	mov %rsp, %rbp

	xor %rax, %rax

	pop %rbp
	retq
.size gcd, .-gcd
