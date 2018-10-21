#!/bin/sh

# install bison && flex
# make shell script executable

ORIG_PATH=`pwd`
TEST_PATH="$ORIG_PATH/tests/"
RESULT_PATH="$ORIG_PATH/results/"

ASM_DRIVER="./x86-64-driver.sh"
C_DRIVER="./c-driver.sh"
DRIVER=""
FILE_ENDING=""

SRC_FILE=""
BIN_FILE=""

# test files
TEST_FILE="looptest"		# grade D


# run_test <driver> <file ending> <test path> <result path> <test>
run_test()
{

	$1 "$3$5.calc"

	if [ $? -eq 0 ]; then
		SRC_FILE=`find ./ -name $5$2`
		BIN_FILE=`find ./ -name $5`

		if [ -e "$SRC_FILE" ]; then
			cp $SRC_FILE "$4"
		fi
		if [ -e "$BIN_FILE" ]; then
			$BIN_FILE > "$4/$5"
		fi
	fi
}

if [ $# -lt 3 ]; then
	echo "usage: $0 [a|c] <path> <run-name> (test-program)"
	exit 1
fi

if [ $# -eq 4 ]; then
		TEST_FILE="$4"
fi

cd "$2"

# run either assembly or c driver

if [ "$1" = "a" ]; then
	DRIVER=$ASM_DRIVER
	FILE_ENDING=".s"
elif [ "$1" = "c" ]; then
	DRIVER=$C_DRIVER
	FILE_ENDING=".c"
else
	echo "only a or c are allowed as driver options"
	exit 2
fi

rm -rf "$RESULT_PATH$3" || true
mkdir -p "$RESULT_PATH$3"

run_test $DRIVER $FILE_ENDING "$TEST_PATH" "$RESULT_PATH/$3" $TEST_FILE
