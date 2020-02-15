#!/bin/sh

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
		export RISCV64=
		#export RISCV64=1
	fi
	if [ "x${arch}" = "xarm64" ]; then
		export SUBARCH=arm64
	fi

	echo "Building ${arch} ${prog}..."
	make ${prog}_defconfig > /dev/null
	make clean > /dev/null
	make > /dev/null
	if [ $? != 0 ]; then
		echo "Building ${prog} failure."
		exit -1
	fi
	echo "Building ${prog} success."
}

build_sdfirm arm64 qdf2400_imc
#build_sdfirm arm64 gem5_boot
#build_sdfirm arm64 gem5_firm
build_sdfirm arm64 gem5_raven
build_sdfirm arm64 cortexa53_tb
build_sdfirm arm64 ddr_umctl2_tb
#build_sdfirm arm64 nanopi_neo2_spl

build_sdfirm riscv32 spike_rv32
build_sdfirm riscv32 duowen_zsbl
build_sdfirm riscv32 duowen_fsbl
build_sdfirm riscv64 duowen_bbl
build_sdfirm riscv64 duowen_sv39
build_sdfirm riscv64 duowen_sv48
# disabled due to bin2vhx.pl bug
#build_sdfirm riscv32 vega_ri5cy

build_sdfirm riscv64 spike_rv64
build_sdfirm riscv64 unleashed_fsbl
build_sdfirm riscv64 unleashed_bbl
build_sdfirm riscv64 k210_bbl
