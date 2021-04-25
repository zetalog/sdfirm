#!/bin/sh
#
# Build litmus images to the bench folder.

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`

LITMUS_CORES=$1
if [ "x${LITMUS_CORES}" = "x" ]; then
	LITMUS_CORES=4
fi
if [ -z ${LITMUS_ROOT} ]; then
	LITMUS_ROOT=${HOME}/workspace/memory-model/litmus-tests-riscv
fi
if [ -z ${CROSS_COMPILE} ]; then
	export CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
LITMUS_SCPT=${TOP}/obj/bench/litmus.sh
LITMUS_SRCS=${LITMUS_ROOT}/litmus-${LITMUS_CORES}cores

if [ -d ${LITMUS_SRCS} ]; then
	# hw-tests-src extracted version
	echo "Generating ${LITMUS_CORES}cores litmus binary from source..."
	(
		cd ${LITMUS_SRCS}
		make -f Makefile.litmus GCC=${CROSS_COMPILE}gcc clean >/dev/null
		make -f Makefile.litmus GCC=${CROSS_COMPILE}gcc
	)
	LITMUS_TAR=no
	LITMUS_RUN=${LITMUS_SRCS}
else
	# git repository version
	echo "Generating ${LITMUS_CORES}cores litmus binary from repository..."
	(
		cd ${LITMUS_ROOT}
		make hw-tests CORES=${LITMUS_CORES} GCC=${CROSS_COMPILE}gcc clean >/dev/null
		make hw-tests CORES=${LITMUS_CORES} GCC=${CROSS_COMPILE}gcc
	)
	LITMUS_TAR=yes
	LITMUS_RUN=${LITMUS_SRCS}/hw-tests
fi
mkdir -p ${TOP}/obj/bench
cp -f ${LITMUS_RUN}/run.exe ${TOP}/obj/bench/litmus
${CROSS_COMPILE}strip ${TOP}/obj/bench/litmus
cp -f ${LITMUS_RUN}/run.sh ${LITMUS_SCPT}.0
sed "s/\.\/run\.exe/litmus/" ${LITMUS_SCPT}.0 > ${LITMUS_SCPT}.1
sed "s/> .*//" ${LITMUS_SCPT}.1 > ${LITMUS_SCPT}.2
mv -f ${LITMUS_SCPT}.2 ${LITMUS_SCPT}
rm -f ${LITMUS_SCPT}.*
chmod +x ${LITMUS_SCPT}

if [ "x${LITMUS_TAR}" = "xyes" ]; then
	echo "Generating ${LITMUS_CORES}cores litmus tarball..."
	rm -rf ${LITMUS_SRCS}
	mv ${LITMUS_ROOT}/hw-tests-src ${LITMUS_SRCS}
	(
		cd ${LITMUS_SRCS}
		make -f Makefile.litmus GCC=${CROSS_COMPILE}gcc clean >/dev/null
	)
	(
		cd ${LITMUS_ROOT}
		tar -zcf litmus-${cores}cores.tgz litmus-${cores}cores
	)
fi
