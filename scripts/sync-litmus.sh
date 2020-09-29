#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd $SCRIPT/..; pwd)`

LITMUS_TSTS=$HOME/workspace/memory-model/litmus-tests-riscv
LITMUS_SRCS=$SRCDIR/tests/riscv/litmus/
LITMUS_CPUS=4

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-t path] [mode]"
	echo "Where:"
	echo " -c num-cpus: specify number of CPUs"
	echo " -t test-dir: specify litmus-tests-riscv directory"
	echo "mode:         specify script execution mode"
	echo "       build: the default mode, generate files"
	echo "       clean: remove C files"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "c:t:" opt
do
	case $opt in
	c) LITMUS_CPUS=$OPTARG;;
	t) LITMUS_TSTS=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	LITMUS_MODE=build
else
	LITMUS_MODE=$1
fi

(
	if [ "x$LITMUS_MODE" = "xbuild" ]; then
		cd $LITMUS_TSTS

		LITMUS_FLAGS="-mode sdfirm -mach ./riscv.cfg \
			-avail $LITMUS_CPUS -o $LITMUS_SRCS"
		if [ -e instructions.excl ]; then
			LITMUS_FLAGS = "$LITMUS_FLAGS -excl instructions.excl"
		fi
		echo "Generating litmus test source..."
		litmus7 $LITMUS_FLAGS "tests/non-mixed-size/@all"
	fi
	if [ "x$LITMUS_MODE" = "xclean" ]; then
		cd $LITMUS_SRCS

		echo "Removing generated litmus tests..."
		echo "# Auto Generated Tests" > Makefile.litmus
		echo "#" > Kconfig.litmus
		echo "# Auto Generated Tests" >> Kconfig.litmus
		echo "#" >> Kconfig.litmus
		find . -not -name "dummy.c" -and -name "*.c" | xargs rm -f
		find . -not -name "dummy.rel" -and -name "*.rel" | xargs rm -f
		rm -f *.h
		rm -f *.awk
		rm -f *.sh
		rm -f src
	fi
)
