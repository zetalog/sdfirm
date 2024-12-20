#!/bin/bash

ARCH=riscv
MACH=spike64
BBLVAR=
CROSS_COMPILE=riscv64-unknown-linux-gnu-

if [ -d "${PWD}/sdfirm" ];then
    SDFIRM_ROOT=${PWD}/sdfirm
else
    SDFIRM_ROOT=${PWD}
fi

BUILD_ROOT=${SDFIRM_ROOT}/../
BUILD_CUSTOMER=
BUILD_TINY=yes
BUILD_LIB=no
BUILD_SMP=yes
BUILD_UEFI=no
BUILD_NET=no
BUILD_STO=no
BUILD_HTIF=no
FORCE_REBUILD=yes

LITMUS_CORES=4
LITMUS_UPDATE=yes
LITMUS_DUMP=no
LITMUS_ROOT=$BUILD_ROOT/memory-model/litmus-tests-riscv

CPU2006_UPDATE=yes
CPU2006_BENCHMARKS=""

PERF_UPDATE=yes

# Early tests include benchmark, litmus, cpu2006
TEST_EARLY=
TEST_LATE=

. ${SDFIRM_ROOT}/scripts/linux/build_sdfirm_linux.sh
