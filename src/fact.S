.code64
.text

.global fact
.type fact, %function
fact: # %rdi = factor
	push %rbp
	mov %rsp, %rbp

	// %rax = output
	mov $1, %rax
	// %rsi = The value output will be multiplied with.
	mov $1, %rsi
1:
	imul %rsi
	inc %rsi
	cmp %rdi, %rsi
	jna 1b

	pop %rbp
	retq

.size fact, .-fact
