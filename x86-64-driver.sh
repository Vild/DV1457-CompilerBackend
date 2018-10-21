#!/bin/bash

set -e

if [ $# -ne 1 ]; then
	echo "$0: <file.calc>"
	exit 1
fi

OUTPUT="${1/.calc/}"
OUTPUT=$(basename $OUTPUT)
TMPFILE="/tmp/calc3-$RANDOM.S"

function prologue() {
	cat <<EOF
.code64
.text

// syscall numbers
#include <asm/unistd_64.h>

.global fact
.global gcd
.global lntwo

.global _start
.type _start, %function
_start:
	mov \$0, %rbp
	call main
	mov %rax, %rdi
	movq \$__NR_exit, %rax
	syscall
.size ., .-_start

.global main
.type main, %function
main:
	pushq %rbp
	mov %rsp, %rbp
EOF
}

function compile() {
	bin/calc3i < $1
	if [ $? -ne 0 ]; then
		echo "Compiler failure: $?" >&2
		exit $?
	fi
}

function epilogue() {
	cat <<EOF
.size main, .-main
	xor %rax, %rax
	pop %rbp
	ret

.global print
.type print, %function
print: // %rdi number to print
	push %rbp
	mov %rsp, %rbp

	xor %rcx, %rcx

	mov %rdi, %rax
	mov \$10, %r8
	mov \$print_buffer_end, %rdi
	inc %rcx
	dec %rdi
	movb $'\n', (%rdi)

	// Is negative?
	cmp \$0, %rax
	jge 1f

	neg %rax
	mov \$1, %r9
	jmp 2f
1:
	mov \$0, %r9
2:

	/*
	 * %rcx = Length
	 * %rax = Number to print
	 * %r8 = DIV does not allow imm values
	 * %rdi = String
	 */
1:
	inc %rcx // length++
	dec %rdi
	xor %rdx, %rdx // %rax is now a unsigned value, so just set rdx to zero
	div %r8 // %rdx = %rax % 10; %rax = %rax / 10
	add \$'0', %rdx
	push %rax
	mov %rdx, %rax
	movb %al, (%rdi)
	pop %rax

	test %rax, %rax // %rax != 0
	jnz 1b

	mov %r9, %rax
	test %rax, %rax
	jz 1f

	inc %rcx
	dec %rdi
	movb \$'-', (%rdi)
1:

	mov %rdi, %rsi // string
	movq \$__NR_write, %rax // sys number
	movq \$1, %rdi // stdout
	mov %rcx, %rdx // Length
	syscall

	xor %rax, %rax
	pop %rbp
	ret
.size print, .-print

.bss

.global print_buffer
print_buffer:
	.space 32 // This will be more than enought to store a UINT64_MAX number.
print_buffer_end:
EOF
	for c in {a..z}; do
		echo ".global $c"
		echo "$c: .quad 0"
	done
}

prologue > $TMPFILE
compile $1 >> $TMPFILE
epilogue >> $TMPFILE

# cat $TMPFILE

gcc -o $OUTPUT -static -nostdlib $TMPFILE lib/libmath.a
rm $TMPFILE
