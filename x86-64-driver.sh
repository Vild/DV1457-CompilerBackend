#!/bin/bash

if [ $# -ne 1 ]; then
		echo "$0: <file.calc>"
		exit 1
fi

OUTPUT="${1/.calc/}"
OUTPUT=$(basename $OUTPUT)

function prologue() {
		cat <<EOF
.code64
.text
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

EOF
}

(
		prologue;
		compile $1;
		epilogue
) | cat

#gcc -x assembler-with-cpp -o $OUTPUT -
