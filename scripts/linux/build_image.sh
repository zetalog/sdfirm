#!/bin/sh

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`

if [ -z ${ARCH} ]; then
	export ARCH=riscv
fi
if [ -z ${MACH} ]; then
	export MACH=spike
fi
if [ -z ${CROSS_COMPILE} ]; then
	export CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
if [ -z ${BBL} ]; then
	export BBL=sdfirm
fi
if [ -z ${SDFIRM_DIR} ]; then
	export SDFIRM_DIR=${WORKING_DIR}/sdfirm
fi
if [ -z ${LINUX_DIR} ]; then
	export LINUX_DIR=${WORKING_DIR}/linux
fi
if [ -z ${BUSYBOX_DIR} ]; then
	export BUSYBOX_DIR=${WORKING_DIR}/busybox
fi

# Build default applications
mkdir -p ${TOP}/obj/bench
APPDIRS="bench net"
APPELFS="		\
	bench/dhrystone	\
	bench/linpack	\
	net/loopback	\
"

for d in ${APPDIRS}; do
	(
	cd ${SDFIRM_DIR}/tests/$d
	make -f Makefile.target clean
	make -f Makefile.target
	)
done
for f in ${APPELFS}; do
	cp -f ${SDFIRM_DIR}/tests/${f}.elf ${TOP}/obj/bench/`basename ${f}`
done

${SCRIPT}/build_module.sh $1

cp -f ${TOP}/obj/linux-riscv/arch/${ARCH}/boot/Image ${SDFIRM_DIR}/Image
