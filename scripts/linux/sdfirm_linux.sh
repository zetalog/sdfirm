#!/bin/bash
#
# This scripts build sdfirm bootable Linux image with early/late test
# specified.

SCRIPT=`(cd ${SDFIRM_ROOT}/scripts/linux; pwd)`

. ${SCRIPT}/sdfirm_tests.sh
. ${SCRIPT}/sdfirm_progs.sh

# Customer support
if [ -z ${BUILD_CUSTOMER} ]; then
	# Enable busybox and applications
	S_MODE=yes
	U_MODE=yes
	HOSTNAME=sdfirm
else
	# Enable busybox only
	S_MODE=yes
	HOSTNAME=sdfirm
fi
M_MODE=yes

usage()
{
	echo "Usage:"
	echo "`basename $0` [-c cores] [-n host]"
	echo "              [-b spec] [-i input]"
	echo "              [-d feat] [-e feat] [-r]"
	echo "              [-m bbl] [-f boot] [target]"
	echo "Where:"
	echo " -m bbl:      enable machine mode program builds (default sdfirm)"
	echo "              bbl includes:"
	echo "    sdfirm:   SDFIRM modified opensbi"
	echo "    riscv-pk: original BBL program"
	echo "    opensbi:  RISC-V opensbi"
	echo " -f:          enable boot firmware builds (default uboot)"
	echo "    uboot:    Das U-Boot linux bootloader"
	echo "    edk2:     UEFI EDK-II BIOS"
	echo " -s:          enable kernel space program builds"
	echo " -u:          enable user space program builds"
	echo " -n host:     specify hostname (default sdfirm)"
	echo " -c cores:    specify number of CPUs to enable litmus"
	echo " -b spec:     specify CPU2006 benchmark to enable specCPU"
	echo " -i input:    specify CPU2006 input data size"
	echo "              input data set includes:"
	echo "    all:      all data sets"
	echo "    test:     data for simple executable test"
	echo "    ref:      data used for generating reports"
	echo "    train:    data for feedback-directed optimization"
	echo " -d feat:     disable special features"
	echo "              feature includes:"
	echo "    shared:   shared library support"
	echo "    smp:      SMP support in OSen"
	echo "    network:  network and telnet login support"
	echo "    storage:  storage and NVME rootfs support"
	echo " -e feat:     disable special features"
	echo "              feature includes:"
	echo "    tiny:     kernel image as tiny as possible"
	echo "    kvm:      KVM virtualization support"
	echo " -r:          enable force module rebuild"
	echo " target:      specify build type (default build)"
	echo "  build       build specified modules (default mode)"
	echo "  clean       build specified modules"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "b:c:d:e:f:i:m:n:r" opt
do
	case $opt in
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
	   fi
	   if [ "x$OPTARG" = "xkvm" ]; then
		BUILD_KVM=yes
	   fi;;
	m) M_MODE=yes
	   BBL=$OPTARG;;
	f) B_MODE=yes
	   BOOT=$OPTARG;;
	i) CPU2006_OPTS="${CPU2006_OPTS} -d $OPTARG";;
	n) HOSTNAME=$OPTARG;;
	r) FORCE_REBUILD=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

# The initramfs should be installed when this command is executed, but may
# be skipped during the KVM builds.
INSTALL_INITRAMFS=yes

# Everything is generated under TOP
cd $TOP

# Start the build
if [ ! -z ${BBLVAR} ]; then
	cp -f ${SDFIRM_ROOT}/arch/riscv/configs/${MACH}_bbl${BBLVAR}_defconfig \
	      ${SDFIRM_ROOT}/arch/riscv/configs/${MACH}_bbl_defconfig
fi

echo $APPDIR
mkdir -p ${APPDIR}

echo "== Prepare =="
if [ "x${M_MODE}" = "xyes" ]; then
	if [ -z $BBL ]; then
		BBL=riscv-pk
	fi
	if [ "x$BBL" = "xsdfirm" ]; then
		if [ -z $SDFIRM_DIR ]; then
			SDFIRM_DIR=sdfirm
			SDFIRM_PATH=$TOP/sdfirm
		else
			if [ ! -d $SDFIRM_DIR ]; then
				echo "Sdfirm source $SDFIRM_DIR not found"
				exit 1
			fi
			SDFIRM_PATH=`(cd $SDFIRM_DIR; pwd)`
			SDFIRM_DIR=`dirname $SDFIRM_PATH`
		fi
		if [ -z $MACH ]; then
			MACH=spike64
		fi
	fi
fi
if [ "x${S_MODE}" = "xyes" ]; then
	if [ ! -f $SCRIPT/config/$LINUX_CONFIG ]; then
		echo "Linux config not found $LINUX_CONFIG"
		exit 1
	fi
	if [ ! -d $LINUX_DIR ]; then
		echo "Linux source $LINUX_DIR not found"
		exit 1
	fi
fi
if [ "x${U_MODE}" = "xyes" ]; then
	if [ ! -f $SCRIPT/config/$BUSYBOX_CONFIG ]; then
		echo "Busybox config not found $BUSYBOX_CONFIG"
		exit 1
	fi
	if [ ! -d $BUSYBOX_DIR ]; then
		echo "Busybox source $BUSYBOX_DIR not found"
		exit 1
	fi
	BUILD_APPS=yes
	BUILD_LITMUS=yes
fi

if [ "x$1" = "xclean" ]; then
	clean_all
else
	build_apps
	if [ -z ${BUILD_CUSTOMER} ]; then
		echo "Building sdfirm..."
		build_test
	else
		echo "Building ${BUILD_CUSTOMER}..."
		build_customer
	fi
	if [ "x${U_MODE}" = "xyes" ]; then
		if [ "x${BUILD_KVM}" = "xyes" ]; then
			build_libfdt
			build_kvmtool
			build_initramfs ${BUILD_STO_SIZE}
			build_linux v
		fi
		build_busybox
		build_initramfs ${BUILD_STO_SIZE}
	fi
	if [ "x${S_MODE}" = "xyes" ]; then
		build_linux
	fi
	if [ "x${M_MODE}" = "xyes" ]; then
		build_bbl
		if [ "x${B_MODE}" = "xyes" ]; then
			build_boot
		fi
	fi
fi

backup_elfs
