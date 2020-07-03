#!/bin/sh

update_defconfigs=

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
		cp -f ./.config arch/${SUBARCH}/configs/${prog}_defconfig
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
	echo "`basename $0` [-u]"
	echo "Where:"
	echo " -u:          update default configurations"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "hu" opt
do
	case $opt in
	h) usage 0;;
	u) update_defconfigs=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

PSEUDO_IMAGE=no
if [ ! -x Image ]; then
	echo "Creating pseudo image..."
	touch Image
	PSEUDO_IMAGE=yes
fi
build_sdfirm riscv64 spike64_bbl
build_sdfirm riscv64 virt64_bbl
build_sdfirm riscv64 unleashed_bbl
build_sdfirm riscv64 k210_bbl
build_sdfirm riscv64 duowen_bbl
if [ "x$PSEUDO_IMAGE" = "xyes" ]; then
	echo "Deleting pseudo image..."
	rm -f Image
fi

build_sdfirm arm64 qdf2400_imc
build_sdfirm arm64 gem5_boot
# disabled due to external GEM5 requirement
#build_sdfirm arm64 gem5_slice
build_sdfirm arm64 gem5_bench
build_sdfirm arm64 cortexa53_tb
build_sdfirm arm64 ddr_umctl2_tb
# disabled due to spl size limitation
#build_sdfirm arm64 nanopi_neo2_spl

build_sdfirm riscv32 spike32_tb
build_sdfirm riscv32 vega_ri5cy

build_sdfirm riscv64 spike64_tb
build_sdfirm riscv64 spike64_nocon
build_sdfirm riscv64 virt64_tb
build_sdfirm riscv64 vaisra_tb
build_sdfirm riscv64 vaisra_nocon
build_sdfirm riscv64 vaisra_fsbl

build_sdfirm riscv64 unleashed_zsbl
build_sdfirm riscv64 unleashed_fsbl

# DUOWEN
build_sdfirm riscv64 duowen_imc_zsbl
build_sdfirm riscv64 duowen_imc_fsbl
build_sdfirm riscv64 duowen_apc_fsbl
# DPU
build_sdfirm riscv64 dpu_rom
build_sdfirm riscv64 dpu_flash
build_sdfirm riscv64 dpu_ram
build_sdfirm riscv64 dpu_ddr
