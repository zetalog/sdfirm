#!/bin/sh
#
# Synchronize the generated litmus cases to the tests/riscv/litmus folder.

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`

LITMUS_TSTS=${HOME}/workspace/memory-model/litmus-tests-riscv
LITMUS_SRCS=${SRCDIR}/tests/riscv/litmus/
LITMUS_CPUS=4
LITMUS_DTCS=yes
CROSS_COMPILE=riscv64-unknwon-linux-gnu-

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-t path] [-g cross] [-x] [mode]"
	echo "Where:"
	echo " Options:"
	echo " -c num-cpus: specify number of CPUs"
	echo "              default is ${LITMUS_CPUS}"
	echo " -t test-dir: specify litmus-tests-riscv directory"
	echo "              default is ${LITMUS_TSTS}"
	echo " -g cross:    specify GCC cross compiler prefix"
	echo "              default is ${CROSS_COMPILE}"
	echo " -x:          disable GCC supports detection"
	echo " Parameters:"
	echo " mode:        specify script execution mode"
	echo "  build:      the default mode, generate files"
	echo "  clean:      remove C files"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "g:c:t:x" opt
do
	case $opt in
	c) LITMUS_CPUS=$OPTARG;;
	t) LITMUS_TSTS=$OPTARG;;
	g) CROSS_COMPILE=$OPTARG;;
	x) LITMUS_DTCS=no;;
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
	if [ "x${LITMUS_MODE}" = "xbuild" ]; then
		cd ${LITMUS_TSTS}

		echo "Generating litmus source to ${LITMUS_SRCS}..."
		LITMUS_FLAGS="-mode sdfirm -mach ./riscv.cfg \
			-avail ${LITMUS_CPUS} -o ${LITMUS_SRCS}"
		if [ "x${LITMUS_DTCS}" = "xyes" ]; then
			${SCRIPT}/excl-litmus.sh -c ${LITMUS_CPUS} -t ${LITMUS_TSTS} -g ${CROSS_COMPILE}
			if [ -e ${CROSS_COMPILE}.excl ]; then
				LITMUS_FLAGS="${LITMUS_FLAGS} -excl ${CROSS_COMPILE}.excl"
			fi
		fi
		if [ -e instructions.excl ]; then
			LITMUS_FLAGS="${LITMUS_FLAGS} -excl instructions.excl"
		fi
		cp -f ${LITMUS_SRCS}/run.c ${SCRIPT}/run.c.backup
		litmus7 ${LITMUS_FLAGS} "tests/non-mixed-size/@all" 2>/dev/null
	fi
	if [ "x${LITMUS_MODE}" = "xclean" ]; then
		cd ${LITMUS_SRCS}

		echo "Removing litmus source from ${LITMUS_SRCS}..."
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
		cp -f ${SCRIPT}/run.c.backup ${LITMUS_SRCS}/run.c
	fi
)
