#!/bin/sh
#
# Synchronize/generate/run litmus cases.

MACH=spike64
SCRIPT=`(cd \`dirname $0\`; pwd)`
LITMUS_TSTS=${HOME}/workspace/memory-model/litmus-tests-riscv
LITMUS_CPUS=4
LITMUS_MODEL=herd

LITMUS_GEN=no
LITMUS_STRIDE=2
LITMUS_SIZE_OF_TEST=5
LITMUS_MAX_RUN=1

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cpus] [-f] [-g] [-m mach]"
	echo "          [-r max_run] [-s stride] [-t size_of_test]"
	echo "          [-p path] [test]"
	echo "Where:"
	echo " -c num-cpus: specify number of CPUs"
	echo "              default is ${LITMUS_CPUS}"
	echo " -f:          specify flat model results to compare"
	echo "              default is herd"
	echo " -g:          generate case elf files"
	echo "              default is ${LITMUS_GEN}"
	echo " -m mach:     specify \${mach}_litmus_defconfig"
	echo "              default is ${MACH}"
	echo " -p test-dir: specify litmus-tests-riscv directory"
	echo "              default is ${LITMUS_TSTS}"
	echo "Options with -g:"
	echo " -r max_run:      litmus max runs (-r)"
	echo "                  default is ${LITMUS_MAX_RUN}"
	echo " -s stride:       litmus stride (-st)"
	echo "                  default is ${LITMUS_STRIDE}"
	echo " -t size_of_test: litmus size of test (-s)"
	echo "                  default is ${LITMUS_SIZE_OF_TEST}"
	echo "test:         specify test, or all tests if omitted"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

run_litmus()
{
	echo "----- $1 -----" | tee -a ${LITMUS_LOG}
	rslt=`${SCRIPT}/run-${SMACH}.sh \
		-p${LITMUS_CPUS} ${SRCDIR}/$1.elf \
		| tee ${SRCDIR}/$1.log | \
		grep "success\|failure" --binary-file=text`
	if [ "x${rslt}" = "xTest success." ]; then
		echo "$1" >> ${SRCDIR}/succ
	else
		echo "$1" >> ${SRCDIR}/fail
	fi
	rslt=`mcompare7 -nohash ${SRCDIR}/$1.log \
		${LITMUS_TSTS}/model-results/${LITMUS_MODEL}.logs \
		| tee ${SRCDIR}/$1.cmp | \
		grep "negative" --binary-file=text | cut -d' ' -f3`
	if [ "x${rslt}" = "xnegative" ]; then
		echo "$1 negative." | tee -a ${LITMUS_LOG}
		echo "$1" >> ${SRCDIR}/nega
	else
		echo "$1 positive." | tee -a ${LITMUS_LOG}
		echo "$1" >> ${SRCDIR}/posi
	fi
}

while getopts "c:gm:p:r:s:t:" opt
do
	case $opt in
	c) LITMUS_CPUS=$OPTARG;;
	f) LITMUS_MODEL=flat;;
	g) LITMUS_GEN=yes;;
	m) MACH=$OPTARG;;
	p) LITMUS_TSTS=$OPTARG;;
	r) LITMUS_MAX_RUN=$OPTARG;;
	s) LITMUS_STRIDE=$OPTARG;;
	t) LITMUS_SIZE_OF_TEST=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

SRCDIR=${LITMUS_TSTS}/${MACH}-tests-src
LITMUS_LOG=${SRCDIR}/litmus_run.log
mkdir -p ${SRCDIR}
if [ "x${LITMUS_GEN}" = "xyes" ]; then
	${SCRIPT}/sync-litmus.sh -c ${LITMUS_CPUS} -t ${LITMUS_TSTS}
	${SCRIPT}/gen-litmus.sh -m ${MACH} -o ${SRCDIR} \
		-r ${LITMUS_MAX_RUN} -s ${LITMUS_STRIDE} \
		-t ${LITMUS_SIZE_OF_TEST} $1
	${SCRIPT}/sync-litmus.sh clean
fi

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

echo -n "" > ${LITMUS_LOG}
echo -n "" > ${SRCDIR}/succ
echo -n "" > ${SRCDIR}/fail
echo -n "" > ${SRCDIR}/nega
echo -n "" > ${SRCDIR}/posi

if [ -z $1 ]; then
	LITMUS_INCL=`cat ${SRCDIR}/incl`
	if [ -x ${SCRIPT}/run-${SMACH}.sh ]; then
		echo "Running all cases on ${SMACH}..."
		for li in $LITMUS_INCL; do
			run_litmus ${li}
		done
	fi
else
	echo "Running $1 case on ${SMACH}..."
	run_litmus $1
fi
