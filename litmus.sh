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
	echo "Building all litmus cases for ${MACH}..."
fi

SRCDIR=${LITMUS_TSTS}/${MACH}-tests-src
mkdir -p ${SRCDIR}
${SCRIPT}/scripts/sync-litmus.sh -c $LITMUS_CPUS -t $LITMUS_TSTS
${SCRIPT}/scripts/gen-litmus.sh -m ${MACH} -o ${SRCDIR} $1
${SCRIPT}/scripts/sync-litmus.sh clean

if [ "x${MACH}" = "xspike64" ]; then
	SMACH=spike
fi
if [ "x${MACH}" = "xspike32" ]; then
	SMACH=spike
fi
if [ "x${MACH}" = "xvirt64" ]; then
	SMACH=qemu
fi
if [ "x${SMACH}" = "x" ]; then
	exit 0;
fi

LITMUS_INCL=`cat $SRCDIR/incl`
if [ -x $SCRIPT/scripts/run-${SMACH}.sh ]; then
	echo "Running all litmus cases on ${SMACH}..."
	echo -n "" > ${SRCDIR}/succ
	echo -n "" > ${SRCDIR}/fail
	echo -n "" > ${SRCDIR}/litmus_run.log
	for li in $LITMUS_INCL; do
		rslt=`$SCRIPT/scripts/run-${SMACH}.sh \
			-p${LITMUS_CPUS} ${SRCDIR}/${li}.elf \
			| tee -a ${SRCDIR}/litmus_run.log | \
			grep "success\|failure" --binary-file=text`
		if [ "x${rslt}" = "xTest success." ]; then
			echo "$li" >> ${SRCDIR}/succ
		else
			echo "$li" >> ${SRCDIR}/fail
		fi
	done
fi
