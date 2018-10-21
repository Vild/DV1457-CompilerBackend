.code64
.text

/*
 * Basicall this:
 * while(a!=n) {
 *  if(a > b)
 *    a -= b;
 *  else
 *    b -= a;
 * }
	*/

.global gcd
.type gcd, %function
gcd: # %rdi = a, %rsi = b
	push %rbp
	mov %rsp, %rbp

.loopStart:
	cmp %rsi, %rdi
	je .loopEnd
	jle .a_lessEq_b

	sub %rdi, %rsi
	jmp .loopStart

.a_lessEq_b:
	sub %rsi, %rdi
	jmp .loopStart

.loopEnd:
	mov %rdi, %rax

	pop %rbp
	retq
.size gcd, .-gcd
