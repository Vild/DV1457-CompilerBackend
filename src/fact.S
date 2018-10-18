.code64
.text

.global fact
.type fact, %function
fact: # %rdi = factor
	push %rbp
	mov %rsp, %rbp

	mov $1, %rax
	mov $1, %rsi
1:
	mul %rsi
	inc %rsi
	cmp %rdi, %rsi
	jna 1b

	pop %rbp
	retq

.size fact, .-fact
