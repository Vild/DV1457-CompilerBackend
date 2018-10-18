#!/bin/bash

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
print:
	pushq %rbp
	mov %rsp, %rbp

	xor %rax, %rax
	pop %rbp
	ret
.size print, .-print

.bss
EOF
	for c in {a..z}; do
		echo "$c: .quad 0"
	done
}

(
	prologue;
	compile $1;
	epilogue
) | tee $TMPFILE

gcc -o $OUTPUT -static -nostdlib $TMPFILE lib/libmath.a
rm $TMPFILE
