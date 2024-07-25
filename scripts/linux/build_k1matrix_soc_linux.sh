#!/bin/bash

ARCH=riscv
MACH=k1matrix
BBLVAR=
CROSS_COMPILE=riscv64-unknown-linux-gnu-

BUILD_ROOT=$HOME/workspace
BUILD_CUSTOMER=
BUILD_TINY=no
BUILD_LIB=no
BUILD_SMP=no
BUILD_NET=no
BUILD_STO=no
BUILD_KVM=no

# If you only want to re-build kernel drivers, you can use no to omit
# rebuilding whole busybox/linux
FORCE_REBUILD=yes

# yes: Baize emulation
# no:  Xihe emulation
SPACET_S2C_SPEEDUP=no
# Supports 4/8
SPACET_CPU=4
# Supports 256m/512m/1g/2g/3g
SPACET_DDR=3g

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
