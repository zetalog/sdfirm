#!/bin/sh

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`
BUILD_MODULE_OPS="-s -u"
BUILD_TINY=no
BUILD_LIB=yes
BUILD_SMP=yes
BUILD_NET=yes
BUILD_STO=yes

usage()
{
	echo "Usage:"
	echo "`basename $0` [-a arch] [-c cores] [-f] [-m mach] [-n host] [-u]"
	echo "              [-d feat] [-e feat] [-r]"
	echo "Where:"
	echo " -a arch:     specify architecture"
	echo " -m mach:     specify machine type"
	echo " -n host:     specify hostname (default sdfirm)"
	echo " -f:          enable firmware builds"
	echo " -u:          enable userspace program builds"
	echo " -c cores:    specify number of CPUs to enable litmus"
	echo " -b spec:     specify CPU2006 benchmark to enable specCPU"
	echo " -d feat:     disable special features"
	echo "              feature includes:"
	echo "    shared:   shared library support"
	echo "    smp:      SMP support in OSen"
	echo "    network:  network and telnet login support"
	echo "    storage:  storage and NVME rootfs support"
	echo " -e feat:     disable special features"
	echo "              feature includes:"
	echo "    tiny:     kernel image as tiny as possible"
	echo " -r:          enable force module rebuild"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:b:c:d:e:fm:n:ru" opt
do
	case $opt in
	a) ARCH=$OPTARG;;
	b) BUILD_CPU2006=yes
	   CPU2006_OPTS="${CPU2006_OPTS} -b $OPTARG";;
	c) LITMUS=$OPTARG
	   BUILD_LITMUS=yes;;
	d) if [ "x$OPTARG" = "xshared" ]; then
		BUILD_LIB=no
	   fi
	   if [ "x$OPTARG" = "xsmp" ]; then
		BUILD_SMP=no
	   fi
	   if [ "x$OPTARG" = "xnetwork" ]; then
		BUILD_NET=no
	   fi
	   if [ "x$OPTARG" = "xstorage" ]; then
		BUILD_STO=no
	   fi;;
	e) if [ "x$OPTARG" = "xtiny" ]; then
		BUILD_TINY=yes
	   fi;;
	f) BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -m sdfirm";;
	m) MACH=$OPTARG;;
	n) BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -n $OPTARG";;
	r) BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -r";;
	u) BUILD_LITMUS=yes
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
if [ -z ${LITMUS_ROOT} ]; then
	export LITMUS_ROOT=${WORKING_DIR}/litmus-tests-riscv
fi
if [ -z ${CPU2006_ROOT} ]; then
	export CPU2006_ROOT=${WORKING_DIR}/cpu2006
fi
if [ "x${BUILD_LIB}" = "xyes" ]; then
	BENCH_STATIC=n
else
	BENCH_STATIC=y
fi

# Build default applications
APPDIR=${TOP}/obj/bench/usr/local/bin
mkdir -p ${APPDIR}
if [ "x${BUILD_APPS}" = "xyes" ]; then
	APPDIRS="bench net"
	APPELFS="		\
		bench/dhrystone	\
		bench/linpack	\
		bench/coremark	\
		net/loopback	\
	"
	for d in ${APPDIRS}; do
		(
		echo "Generating ${d} applications..."
		cd ${SDFIRM_DIR}/tests/${d}
		make -f Makefile.target clean
		CONFIG_BENCH_STATIC=$BENCH_STATIC make -f Makefile.target
		)
	done
	for f in ${APPELFS}; do
		echo "Creating ${f} application..."
		cp -f ${SDFIRM_DIR}/tests/${f}.elf \
			${APPDIR}/`basename ${f}`
	done
fi
if [ "x${BUILD_TINY}" = "xyes" ]; then
	BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -e tiny"
fi
if [ "x${BUILD_LIB}" = "xno" ]; then
	BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -d shared"
fi
if [ "x${BUILD_SMP}" = "xno" ]; then
	BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -d smp"
fi
if [ "x${BUILD_NET}" = "xno" ]; then
	BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -d network"
fi
if [ "x${BUILD_STO}" = "xno" ]; then
	BUILD_MODULE_OPS="${BUILD_MODULE_OPS} -d storage"
fi

# Build memory model application tests
if [ "x${BUILD_LITMUS}" = "xyes" ]; then
	if [ "x${LITMUS}" != "x" ]; then
		${SCRIPT}/build_litmus.sh -r ${LITMUS}
	fi
fi
# Build SPEC CPU2006 benchmark tests
if [ "x${BUILD_CPU2006}" = "xyes" ]; then
	${SCRIPT}/build_cpu2006.sh -h -t -r ${CPU2006_OPTS}
fi

# Build linux image along with rootfs
${SCRIPT}/build_module.sh ${BUILD_MODULE_OPS} build

cp -f ${TOP}/obj/linux-riscv/arch/${ARCH}/boot/Image ${SDFIRM_DIR}/Image
