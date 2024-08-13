#!/bin/sh
# NOTE: This scripts automatically switches cross compile tools using
#       SUBARCH and uses ARCH default cross compiler prefix. Thus it
#       unsets CROSS_COMPILE, which may affect your environment.

update_defconfigs=
build_all_defconfigs=

build_sdfirm()
{
	arch=$1
	prog=$2

	if [ "x${arch}" = "xriscv32" ]; then
		export SUBARCH=riscv
		export RISCV64=
	fi
	if [ "x${arch}" = "xriscv64" ]; then
		export SUBARCH=riscv
		export RISCV64=1
	fi
	if [ "x${arch}" = "xarm64" ]; then
		export SUBARCH=arm64
	fi

	echo "Building ${arch} ${prog}..."
	make ${prog}_defconfig > /dev/null
	if [ "x$update_defconfigs" = "xyes" ]; then
		pl=`diff -u ./.config arch/${SUBARCH}/configs/${prog}_defconfig | grep '^+' | wc -l`
		ml=`diff -u ./.config arch/${SUBARCH}/configs/${prog}_defconfig | grep '^-' | wc -l`
		if [ $pl -gt 2 -o $ml -gt 2 ]; then
			echo "Updating ${arch} ${prog}..."
			cp -f ./.config \
				arch/${SUBARCH}/configs/${prog}_defconfig
		fi
	fi
	make clean > /dev/null
	make > /dev/null
	if [ $? != 0 ]; then
		echo "Building ${prog} failure."
		exit -1
	fi
	echo "Building ${prog} success."
}

usage()
{
	echo "Usage:"
	echo "`basename $0` [-u] [arch]"
	echo "Where:"
	echo " -u:          update default configurations"
	echo " arch:        architecture, can be"
	echo "              riscv32: RISC-V RISCV32"
	echo "              riscv64: RISC-V RISCV64"
	echo "              arm64:   ARM AARCH64"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "ahu" opt
do
	case $opt in
	a) build_all_defconfigs=yes;;
	h) usage 0;;
	u) update_defconfigs=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

BUILD_ARCH=$1

PSEUDO_IMAGE=no
if [ ! -x Image ]; then
	echo "Creating pseudo image..."
	touch Image
	PSEUDO_IMAGE=yes
fi

# Save environments and clear them to allow build_sdfirm() to determine
# via ${SUBARCH} and ${RISCV64}
SAVED_ARCH=${ARCH}
SAVED_CROSS=${CROSS_COMPILE}
unset ARCH
unset CROSS_COMPILE

if [ -z $BUILD_ARCH ]; then
	BUILD_RISCV32=yes
	BUILD_RISCV64=yes
	BUILD_ARM64=yes
elif [ "x${BUILD_ARCH}" = "xriscv32" ]; then
	BUILD_RISCV32=yes
elif [ "x${BUILD_ARCH}" = "xriscv64" ]; then
	BUILD_RISCV64=yes
elif [ "x${BUILD_ARCH}" = "xarm64" ]; then
	BUILD_ARM64=yes
fi

if [ "x${BUILD_RISCV64}" = "xyes" ]; then
	# All linux booting BBL
	build_sdfirm riscv64 spike64_bbl
	build_sdfirm riscv64 virt64_bbl
	build_sdfirm riscv64 unleashed_bbl
	build_sdfirm riscv64 k210_bbl
	build_sdfirm riscv64 dpu2_bbl
	build_sdfirm riscv64 dpures_bbl
	build_sdfirm riscv64 dpuresm_bbl
	build_sdfirm riscv64 duowen_bbl_apc
	build_sdfirm riscv64 duowen_bbl_imc
	build_sdfirm riscv64 k1max_cpu_bbl
	build_sdfirm riscv64 k1max_soc_bbl
	build_sdfirm riscv64 k1mxlite_bbl
	build_sdfirm riscv64 k1mxlite_d2d_bbl
	if [ "x$PSEUDO_IMAGE" = "xyes" ]; then
		echo "Deleting pseudo image..."
		rm -f Image
	fi
fi

if [ "x${BUILD_ARM64}" = "xyes" ]; then
	build_sdfirm arm64 qdf2400_imc
	build_sdfirm arm64 gem5_boot
	# disabled due to external GEM5 requirement
	#build_sdfirm arm64 gem5_slice
	build_sdfirm arm64 gem5_bench
	build_sdfirm arm64 cortexa53_tb
	build_sdfirm arm64 ddr_umctl2_tb
	# disabled due to spl size limitation
	#build_sdfirm arm64 nanopi_neo2_spl
fi

if [ "x${BUILD_RISCV32}" = "xyes" ]; then
	# RV32M1 Vega
	if [ "x$build_all_defconfigs" = "xyes" ]; then
		build_sdfirm riscv32 vega_ri5cy
	fi
	# Emulators
	build_sdfirm riscv32 spike32_tb
	# core-v-verif
	if [ "x$build_all_defconfigs" = "xyes" ]; then
		build_sdfirm riscv32 corev_cv32
	fi
fi
if [ "x${BUILD_RISCV64}" = "xyes" ]; then
	build_sdfirm riscv64 spike64_tb
	build_sdfirm riscv64 spike64_nocon
	build_sdfirm riscv64 spike64_litmus
	build_sdfirm riscv64 spike64_task
	build_sdfirm riscv64 virt64_tb
	build_sdfirm riscv64 virt64_litmus
	# DUOWEN Vaisra TB
	build_sdfirm riscv64 vaisra_tb
	build_sdfirm riscv64 vaisra_nocon
	build_sdfirm riscv64 vaisra_fsbl
	# Guangmu Virapa TB
	build_sdfirm riscv64 virapa_tb
	# SiFive unleashed
	build_sdfirm riscv64 unleashed_zsbl
	build_sdfirm riscv64 unleashed_fsbl
	#################################################################
	# DUOWEN ROMs
	#################################################################
	build_sdfirm riscv64 duowen_zsbl
	build_sdfirm riscv64 duowen_asbl
	#################################################################
	# DUOWEN bootloaders
	#################################################################
	# ROM FSBL
	build_sdfirm riscv64 duowen_fsbl
	# SPI XSBL
	build_sdfirm riscv64 duowen_xsbl_imc
	build_sdfirm riscv64 duowen_xsbl_apc
	#################################################################
	# DUOWEN simulations
	#################################################################
	build_sdfirm riscv64 duowen_spi_imc
	build_sdfirm riscv64 duowen_spi_apc
	build_sdfirm riscv64 duowen_ram_imc
	build_sdfirm riscv64 duowen_ram_apc
	build_sdfirm riscv64 duowen_ddr
	# DPU products
	build_sdfirm riscv64 dpu_rom
	build_sdfirm riscv64 dpu_ram
	build_sdfirm riscv64 dpu_flash
	# DPU testbenches
	build_sdfirm riscv64 dpu_ddr
	# DPU GEN2 products
	build_sdfirm riscv64 dpu2_rom
	build_sdfirm riscv64 dpu2_ram
	build_sdfirm riscv64 dpu2_flash
	# DPU GEN2 testbenches
	build_sdfirm riscv64 dpu2_ddr
	# DPU RES IMC products
	build_sdfirm riscv64 dpures_rom
	build_sdfirm riscv64 dpures_ram
	build_sdfirm riscv64 dpures_flash
	# DPU RES IMC testbenches
	build_sdfirm riscv64 dpures_ddr
	# DPU RES APC products
	build_sdfirm riscv64 dpuresm_rom
	build_sdfirm riscv64 dpuresm_ram
	build_sdfirm riscv64 dpuresm_flash
	# DPU RES APC testbenches
	build_sdfirm riscv64 dpuresm_ddr
	# K1-max
	build_sdfirm riscv64 k1max_cpu_litmus
	build_sdfirm riscv64 k1max_soc_litmus
	# K1MXLite
	build_sdfirm riscv64 k1mxlite_zsbl
	build_sdfirm riscv64 k1mxlite_fsbl
	build_sdfirm riscv64 k1mxlite_d2d_fsbl
	build_sdfirm riscv64 k1mxlite_litmus
fi

# Restore environments
export ARCH=${SAVED_ARCH}
export CROSS_COMPILE=${SAVED_CROSS}
