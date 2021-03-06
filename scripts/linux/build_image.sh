#!/bin/sh

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`
BUILD_MODULE_OPS="-s"

usage()
{
	echo "Usage:"
	echo "`basename $0` [-a arch] [-c cores] [-f] [-m mach] [-u]"
	echo "Where:"
	echo " -a arch:     specify architecture"
	echo " -m mach:     specify machine type"
	echo " -f:          enable firmware builds"
	echo " -u:          enable userspace program builds"
	echo " -c cores:    specify number of CPUs to enable litmus"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:c:fm:u" opt
do
	case $opt in
	a) ARCH=$OPTARG;;
	c) LITMUS=$OPTARG
	   BUILD_LITMUS=yes;;
	f) BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -m sdfirm";;
	m) MACH=$OPTARG;;
	u) BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -u"
	   BUILD_LITMUS=yes
	   BUILD_APPS=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

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
if [ "x${BUILD_APPS}" = "xyes" ]; then
	APPDIRS="bench net"
	APPELFS="		\
		bench/dhrystone	\
		bench/linpack	\
		net/loopback	\
	"
	for d in ${APPDIRS}; do
		(
		echo "Generating ${d} applications..."
		cd ${SDFIRM_DIR}/tests/${d}
		make -f Makefile.target clean
		make -f Makefile.target
		)
	done
	for f in ${APPELFS}; do
		echo "Creating ${f} application..."
		cp -f ${SDFIRM_DIR}/tests/${f}.elf \
			${TOP}/obj/bench/`basename ${f}`
	done
fi

# Build memory model application tests
if [ "x${BUILD_LITMUS}" = "xyes" ]; then
	if [ "x${LITMUS}" != "x" ]; then
		${SCRIPT}/build_litmus.sh ${LITMUS}
	fi
fi

# Build linux image along with rootfs
${SCRIPT}/build_module.sh ${BUILD_MODULE_OPS} build

cp -f ${TOP}/obj/linux-riscv/arch/${ARCH}/boot/Image ${SDFIRM_DIR}/Image
