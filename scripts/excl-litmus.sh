#!/bin/sh
#
# Detect and exclude non-compilable tests.
# FIXME:
#  This script utilizes litmus-tests-riscv embedded build system to detect
#  GCC support. However that requires the GCC to contain libc support. So
#  if you want to exclude GCC test cases efficiently, nolibc support
#  toolchains cannot be used currently to build sdfirm while they should
#  be able to.

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`
MSORT=msort7
MSUM=msum7
LITMUS=litmus7
ATFILE=tests/non-mixed-size/@all

CROSS_COMPILE=riscv64-unknown-linux-gnu-
LITMUS_CPUS=4
LITMUS_TSTS=${HOME}/workspace/memory-model/litmus-tests-riscv
MACH=spike64

detect_litmus()
{
	TEST=$2
	TESTSUIT=$1
	TESTDIR=${OUTDIR}/${TESTSUIT}-src

	echo "Detecting the GCC support of ${TESTDIR}..."
	mkdir -p ${TESTDIR}
	${LITMUS} -mach ./riscv.cfg -avail ${LITMUS_CPUS} -o ${TESTDIR} ${TEST}
	make -C ${TESTDIR} -f Makefile.litmus clean 1>/dev/null 2>&1
	make -C ${TESTDIR} -f Makefile.litmus CORES=${LITMUS_CPUS} GCC=${CROSS_COMPILE}gcc 1>/dev/null 2>&1
}

grep_tests()
{
	eval "${MSORT} -d ${ATFILE} | grep '^[^\#]' | xargs grep -li $* | sed -e 's|.*/||' -e 's|\.litmus||'"
}

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-m mach] [-t path] [-g cross]"
	echo "Where:"
	echo " Options:"
	echo " -c num-cpus: specify number of CPUs"
	echo "              default is ${LITMUS_CPUS}"
	echo " -t test-dir: specify litmus-tests-riscv directory"
	echo "              default is ${LITMUS_TSTS}"
	echo " -g cross:    specify cross compiler prefix"
	echo "              default is ${CROSS_COMPILE}"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "g:c:t:" opt
do
	case $opt in
	g) CROSS_COMPILE=$OPTARG;;
	c) LITMUS_CPUS=$OPTARG;;
	t) LITMUS_TSTS=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

(
	cd ${LITMUS_TSTS}
	export OUTDIR=${CROSS_COMPILE}tests
	export OUTXCL=${CROSS_COMPILE}.excl

	detect_litmus MP tests/non-mixed-size/BASIC_2_THREAD/MP.litmus
	if [ $? != 0 ]; then
		echo "Error: Failed to build litmus programs"
		exit 1
	fi

	rm -rf ${OUTDIR} ${OUTXCL}

	echo "# `${CROSS_COMPILE}gcc --version | head -1`" >${OUTXCL}
	echo "# tests with l{b|h|w|d}.aq[.rl] or s{b|h|w|d}[.aq].rl instruction:" >>${OUTXCL}
	grep_tests -e l[bhwd]\.aq -e l[bhwd]\.aq\.rl -e s[bhwd]\.rl -e s[bhwd]\.aq\.rl >>${OUTXCL}

	# fence.tso
	detect_litmus fence.tso \
		tests/non-mixed-size/SINGLE_INST/fence.tso.litmus
	if [ $? != 0 ]; then
		echo "Error: fence.tso unsupported."
		echo "# tests with fence.tso instruction:" >>${OUTXCL}
		grep_tests -F fence.tso >>${OUTXCL}
	fi

	# amo*.w|d.aq.rl
	detect_litmus amoswap.w.aq.rl \
		tests/non-mixed-size/SINGLE_INST/amoswap.w.aq.rl.litmus
	if [ $? != 0 ]; then
		echo "Error: amo*.w|d.aq.rl unsupported."
		echo "# tests with amo*.{w|d}.aq.rl instructions:" >>${OUTXCL}
		grep_tests amo[^[:space:]]*\.[wd]\.aq\.rl >>${OUTXCL}
	fi

	# lr|sc.w|d.aq.rl
	detect_litmus lr.w.aq.rl \
		tests/non-mixed-size/SINGLE_INST/lr.w.aq.rl.litmus
	if [ $? != 0 ]; then
		echo "Error: lr|sc.w|d.aq.rl unsupported."
		echo "# tests with {lr|sc}.{w|d}.aq.rl instruction:" >>${OUTXCL}
		grep_tests -e lr\.[wd]\.aq\.rl -e sc\.[wd]\.aq\.rl >>${OUTXCL}
	fi
)
