#!/bin/bash

ARCH=riscv
MACH=k1-x
BBLVAR=
CROSS_COMPILE=riscv64-unknown-linux-gnu-
SDFIRM_ROOT=$HOME/workspace_k1x/sdfirm

BUILD_ROOT=$HOME/workspace_k1x
BUILD_CUSTOMER=stress-ng
# BUILD_CUSTOMER=

BUILD_CPU=8
BUILD_DDR=2g
BUILD_TINY=no
BUILD_LIB=yes
BUILD_SMP=yes
BUILD_NET=yes
BUILD_STO=no

FORCE_REBUILD=yes

LITMUS_CORES=8
LITMUS_UPDATE=no
LITMUS_DUMP=no
LITMUS_ROOT=$HOME/workspace_k1x/memory-model/litmus-tests-riscv

PERF_UPDATE=no
CONFIG_RISCV_ISA_V=y

#CPU2006_UPDATE=yes
#CPU2006_REPORT=specinvoke
#CPU2006_DATA=all
#CPU2006_BENCHMARKS="400.perlbench"

#TEST_EARLY=cpu2006
#TEST_EARLY=
TEST_LATE=

. ${PWD}/sdfirm/scripts/linux/build_sdfirm_linux.sh


