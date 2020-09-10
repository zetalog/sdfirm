#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd $SCRIPT/..; pwd)`

LITMUS_TSTS=$HOME/workspace/memory-model/litmus-tests-riscv
LITMUS_SRCS=$SRCDIR/tests/riscv/litmus/
LITMUS_CPUS=4

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-t path]"
	echo "Where:"
	echo " -c num-cpus: specify number of CPUs"
	echo " -s test-dir: specify litmus-tests-riscv directory"
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

(
	cd $LITMUS_TSTS

	litmus7 -mode sdfirm -mach ./riscv.cfg \
		-avail $LITMUS_CPUS -excl instructions.excl \
		-o $LITMUS_SRCS \
		"tests/non-mixed-size/@all"
)
