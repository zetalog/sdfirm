#!/bin/bash

ARCH=riscv
MACH=k1matrix
BBLVAR=
CROSS_COMPILE=riscv64-unknown-linux-gnu-

if [ -d "${PWD}/sdfirm" ];then
    SDFIRM_ROOT=${PWD}/sdfirm
else
    SDFIRM_ROOT=${PWD}
fi

BUILD_ROOT=${SDFIRM_ROOT}/../
BUILD_CUSTOMER=

# Simulation environments:
# asic: real hardware and EDA environment
# s2c: FPGA S2C
# vps: Veloca ProFPGA
# pz1: Palladium Z1 emulator
# Can be configured for all spacemit platforms
BUILD_SIM=asic
BUILD_CPU=64
# support 4g/8g
BUILD_DDR=4g
BUILD_TINY=yes
BUILD_LIB=no
BUILD_SMP=yes
BUILD_NET=no
BUILD_STO=no
BUILD_KVM=no
BUILD_OCM=yes
BUILD_NUMA=no

# If you only want to re-build kernel drivers, you can use no to omit
# rebuilding whole busybox/linux
FORCE_REBUILD=yes

LITMUS_CORES=4
LITMUS_UPDATE=no
LITMUS_DUMP=no
LITMUS_ROOT=$HOME/workspace/memory-model/litmus-tests-riscv

CPU2006_UPDATE=yes
# : means bare execution using "specinvoke -nn" output
# specinvoke: use specinvoke command to generate output
# runspec: use runspec command to generate output
# no/or anything else: do not execute but only link /usr/bin/perl
#CPU2006_REPORT=
CPU2006_REPORT=specinvoke
#CPU2006_REPORT=runspec
#CPU2006_REPORT=no
CPU2006_DATA=all
CPU2006_BENCHMARKS="401.bzip2"

PERF_UPDATE=no

# Early tests include benchmark, litmus, cpu2006
TEST_EARLY=
TEST_LATE=

. ${SDFIRM_ROOT}/scripts/linux/build_sdfirm_linux.sh
