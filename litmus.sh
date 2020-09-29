#!/bin/sh

MACH=spike64
SCRIPT=`(cd \`dirname $0\`; pwd)`
LITMUS_TSTS=$HOME/workspace/memory-model/litmus-tests-riscv
LITMUS_CPUS=4

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-t path] [test]"
	echo "Where:"
	echo " -c num-cpus: specify number of CPUs"
	echo " -c mach:     specify \${mach}_litmus_defconfig"
	echo " -t test-dir: specify litmus-tests-riscv directory"
	echo "test:         specify test, or all tests if omitted"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "c:m:t:" opt
do
	case $opt in
	c) LITMUS_CPUS=$OPTARG;;
	m) MACH=$OPTARG;;
	t) LITMUS_TSTS=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	echo "Building all litmus cases..."
fi

SRCDIR=${LITMUS_TSTS}/${MACH}-tests-src
mkdir -p ${SRCDIR}
${SCRIPT}/scripts/sync-litmus.sh -c $LITMUS_CPUS -t $LITMUS_TSTS
${SCRIPT}/scripts/gen-litmus.sh -m ${MACH} -o ${SRCDIR} $1
${SCRIPT}/scripts/sync-litmus.sh clean
