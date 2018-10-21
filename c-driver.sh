#!/bin/bash

set -e

if [ $# -ne 1 ]; then
	echo "$0: <file.calc>"
	exit 1
fi

OUTPUT="${1/.calc/}"
OUTPUT=$(basename $OUTPUT)
OUTPUT_SOURCE="$OUTPUT.c"

function prologue() {
	cat <<EOF

// syscall numbers
#include <asm/unistd_64.h>
#include <stdint.h>

extern int64_t fact(int64_t a);
extern int64_t gcd(int64_t a, int64_t b);
extern int64_t lntwo(int64_t a);

int main(void);
static void print(int64_t number);
static void push(int64_t number);
static int64_t pop();
static int64_t stackBuffer[0x1000];
static int stackIndex = -1;

void __attribute__((naked, noreturn)) _start(void) {
	asm("mov \$0, %rbp");
	int result = main();
	asm("syscall" :	: "a"(__NR_exit), "D"(result));
}

EOF
	echo -n "int64_t a = 0"
	for c in {b..z}; do
		echo -n ", $c = 0"
	done
	echo ";"
	cat <<EOF
int main(void) {
EOF
}

function compile() {
	bin/calc3.c < $1
	if [ $? -ne 0 ]; then
		echo "Compiler failure: $?" >&2
		exit $?
	fi
}

function epilogue() {
	cat <<EOF
	return 0;
}


static void print(int64_t number) {
	char print_buffer[32];

	char* str = &print_buffer[32];
	int length = 1;
	*(--str) = '\n';

	int isNeg = number < 0;
	if (isNeg)
		number *= -1;

	do {
		int64_t part = number % 10;
		number /= 10;

		*(--str) = part + '0';
		length++;
	} while (number);

	if (isNeg) {
		*(--str) = '-';
		length++;
	}

	asm("syscall" :	: "a"(__NR_write), "D"(1), "S"(str), "d"(length));
}

static void push(int64_t number) {
	stackIndex++;
	if (stackIndex == sizeof(stackBuffer)/sizeof(stackBuffer[0]))
		asm("syscall" :	: "a"(__NR_exit), "D"(128));
	stackBuffer[stackIndex] = number;
}

static int64_t pop() {
	if (stackIndex == -1)
		asm("syscall" :	: "a"(__NR_exit), "D"(255));
	return stackBuffer[stackIndex--];
}

EOF
}

prologue > $OUTPUT_SOURCE
compile $1 >> $OUTPUT_SOURCE
epilogue >> $OUTPUT_SOURCE

# cat $OUTPUT_SOURCE

gcc -o $OUTPUT -static -nostdlib -fno-stack-protector $OUTPUT_SOURCE lib/libmath.a
