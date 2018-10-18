#!/bin/bash

if [ $# -ne 1 ]; then
		echo "$0: <file.calc>"
		exit 1
fi

OUTPUT="${1/.calc/}"
OUTPUT=$(basename $OUTPUT)

function prologue() {
		cat <<EOF
#include <stdio.h>
#include <stdlib.h>

void main() {
/*
EOF
}

function compile() {
		bin/calc3i <$1
		if [ $? -ne 0 ]; then
			 exit $?
		fi
}

function epilogue() {
		cat <<EOF
*/
	return 0;
}
EOF
}

(
		prologue;
		compile $1;
		epilogue
) | cat

#gcc -x c -o $OUTPUT -
